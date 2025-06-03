// Copyright June Rhodes. All Rights Reserved.

#include "BPNode_EnqueueGameManagerTask.h"

#include "GameManager.h"
#include "GameManagerTask.h"

#include "BlueprintCompilationManager.h"
#include "BlueprintNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "K2Node_CallArrayFunction.h"
#include "K2Node_EnumLiteral.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetCompiler.h"

#define LOCTEXT_NAMESPACE "GameManagementFramework"

struct FBPNode_EnqueueGameManagerTaskHelper
{
    static FName WorldContextPinName;
    static FName ClassPinName;
    static FName ShowLoadingScreenPinName;
    static FName OuterPinName;
};

FName FBPNode_EnqueueGameManagerTaskHelper::WorldContextPinName(TEXT("WorldContextObject"));
FName FBPNode_EnqueueGameManagerTaskHelper::ClassPinName(TEXT("Class"));
FName FBPNode_EnqueueGameManagerTaskHelper::ShowLoadingScreenPinName(TEXT("Show Loading Screen"));
FName FBPNode_EnqueueGameManagerTaskHelper::OuterPinName(TEXT("Outer"));

UBPNode_EnqueueGameManagerTask::UBPNode_EnqueueGameManagerTask(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    NodeTooltip = LOCTEXT("NodeTooltip", "Enqueues a Game Manager Task");
}

void UBPNode_EnqueueGameManagerTask::AllocateDefaultPins()
{
    // Add execution pins
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
    CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
    auto Pin =
        CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UGameManager::StaticClass(), UEdGraphSchema_K2::PN_Self);

    // Hide self pin if it's not needed.
    UBlueprint *MyBlueprint = GetBlueprint();
    auto ParentClass = MyBlueprint->ParentClass;
    if (ParentClass->ImplementsInterface(UGameManager::StaticClass()) || ParentClass == UGameManager::StaticClass())
    {
        // Hide the self pin if we're in a game manager graph.
        Pin->SafeSetHidden(true);
    }

    // If required add the world context pin
    if (UseWorldContext())
    {
        CreatePin(
            EGPD_Input,
            UEdGraphSchema_K2::PC_Object,
            UObject::StaticClass(),
            FBPNode_EnqueueGameManagerTaskHelper::WorldContextPinName);
    }

    // Add blueprint pin
    UEdGraphPin *ClassPin = CreatePin(
        EGPD_Input,
        UEdGraphSchema_K2::PC_Class,
        GetClassPinBaseClass(),
        FBPNode_EnqueueGameManagerTaskHelper::ClassPinName);

    // Result pin
    UEdGraphPin *ResultPin =
        CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Object, GetClassPinBaseClass(), UEdGraphSchema_K2::PN_ReturnValue);

    if (UseOuter())
    {
        UEdGraphPin *OuterPin = CreatePin(
            EGPD_Input,
            UEdGraphSchema_K2::PC_Object,
            UObject::StaticClass(),
            FBPNode_EnqueueGameManagerTaskHelper::OuterPinName);
    }

    const UEdGraphSchema_K2 *K2Schema = GetDefault<UEdGraphSchema_K2>();

    // Show Loading Screen pin
    UEdGraphPin *ShowLoadingScreenPin = CreatePin(
        EGPD_Input,
        UEdGraphSchema_K2::PC_Boolean,
        TEXT(""),
        FBPNode_EnqueueGameManagerTaskHelper::ShowLoadingScreenPinName);
    SetPinToolTip(
        *ShowLoadingScreenPin,
        LOCTEXT("ShowLoadingScreenPinDescription", "Whether to show the loading screen for this task execution"));
}

FText UBPNode_EnqueueGameManagerTask::GetBaseNodeTitle() const
{
    return LOCTEXT("EnqueueGameManagerTask_BaseTitle", "Enqueue Game Manager Task");
}

bool UBPNode_EnqueueGameManagerTask::IsCompatibleWithGraph(const UEdGraph *TargetGraph) const
{
    UBlueprint *Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(TargetGraph);
    return Super::IsCompatibleWithGraph(TargetGraph);
}

FText UBPNode_EnqueueGameManagerTask::GetNodeTitleFormat() const
{
    return LOCTEXT("EnqueueGameManagerTask", "Enqueue {ClassName} Task");
}

UClass *UBPNode_EnqueueGameManagerTask::GetClassPinBaseClass() const
{
    return UGameManagerTask::StaticClass();
}

FText UBPNode_EnqueueGameManagerTask::GetMenuCategory() const
{
    return FText::FromString("Game Management Framework");
}

void UBPNode_EnqueueGameManagerTask::GetMenuActions(FBlueprintActionDatabaseRegistrar &ActionRegistrar) const
{
    UClass *ActionKey = GetClass();

    if (ActionRegistrar.IsOpenForRegistration(ActionKey))
    {
        UBlueprintNodeSpawner *NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
        check(NodeSpawner != nullptr);

        ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
    }
}

UEdGraphPin *UBPNode_EnqueueGameManagerTask::GetShowLoadingScreenPin() const
{
    UEdGraphPin *Pin = FindPin(FBPNode_EnqueueGameManagerTaskHelper::ShowLoadingScreenPinName);
    check(Pin == NULL || Pin->Direction == EGPD_Input);
    return Pin;
}

bool UBPNode_EnqueueGameManagerTask::IsSpawnVarPin(UEdGraphPin *Pin) const
{
    return (
        Super::IsSpawnVarPin(Pin) && !(Pin->PinName == UEdGraphSchema_K2::PN_Self && Pin->Direction == EGPD_Input) &&
        Pin->PinName != FBPNode_EnqueueGameManagerTaskHelper::ShowLoadingScreenPinName);
}

void UBPNode_EnqueueGameManagerTask::ExpandNode(class FKismetCompilerContext &CompilerContext, UEdGraph *SourceGraph)
{
    Super::ExpandNode(CompilerContext, SourceGraph);

    static FName EnqueueTask_FunctionName = GET_FUNCTION_NAME_CHECKED(UGameManager, EnqueueTask);

    static FString WorldContextObject_ParamName = FString(TEXT("WorldContextObject"));
    static FString TaskType_ParamName = FString(TEXT("TaskType"));
    static FString ShowLoadingScreen_ParamName = FString(TEXT("bShowLoadingScreen"));

    UEdGraphPin *SpawnNodeExec = this->GetExecPin();
    UEdGraphPin *SpawnWorldContextPin = this->GetWorldContextPin();
    UEdGraphPin *SpawnTaskTypePin = this->GetClassPin();
    UEdGraphPin *SpawnShowLoadingScreenPin = this->GetShowLoadingScreenPin();
    UEdGraphPin *SpawnNodeThenPin = this->GetThenPin();
    UEdGraphPin *SpawnNodeResult = this->GetResultPin();
    UEdGraphPin *SpawnSelfPin = this->FindPinChecked(UEdGraphSchema_K2::PN_Self, EGPD_Input);

    UClass *SpawnClass = (SpawnTaskTypePin != NULL) ? Cast<UClass>(SpawnTaskTypePin->DefaultObject) : NULL;
    if ((0 == SpawnTaskTypePin->LinkedTo.Num()) && (NULL == SpawnClass))
    {
        CompilerContext.MessageLog.Error(
            *LOCTEXT("EnqueueGameManagerTaskMissingClass_Error", "Spawn node @@ must have a class specified.")
                 .ToString(),
            this);
        this->BreakAllNodeLinks();
        return;
    }

    // Create the actual node that we represent (the call to the function)
    UK2Node_CallFunction *CallTaskNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    CallTaskNode->FunctionReference.SetExternalMember(EnqueueTask_FunctionName, UGameManager::StaticClass());
    CallTaskNode->AllocateDefaultPins();

    // Get references to the pins on that node.
    UEdGraphPin *CallExec = CallTaskNode->GetExecPin();
    UEdGraphPin *CallSelfPin = CallTaskNode->FindPinChecked(UEdGraphSchema_K2::PN_Self, EGPD_Input);
    UEdGraphPin *CallWorldContextPin = CallTaskNode->FindPinChecked(WorldContextObject_ParamName);
    UEdGraphPin *CallTaskTypePin = CallTaskNode->FindPinChecked(TaskType_ParamName);
    UEdGraphPin *CallShowLoadingScreenPin = CallTaskNode->FindPinChecked(ShowLoadingScreen_ParamName);
    UEdGraphPin *CallThenPin = CallTaskNode->GetThenPin();
    UEdGraphPin *CallResult = CallTaskNode->GetReturnValuePin();

    // Map our "Exec In" pin to the node.
    CompilerContext.MovePinLinksToIntermediate(*SpawnNodeExec, *CallExec);

    // Map out "Self" pin to the ScheduleTask node.
    CompilerContext.MovePinLinksToIntermediate(*SpawnSelfPin, *CallSelfPin);

    // Copy the "Task Id" pin to the ScheduleTask node.
    CompilerContext.MovePinLinksToIntermediate(*SpawnShowLoadingScreenPin, *CallShowLoadingScreenPin);

    // Map our "Task Type" pin to the node (or set a default).
    if (SpawnTaskTypePin->LinkedTo.Num() > 0)
    {
        CompilerContext.MovePinLinksToIntermediate(*SpawnTaskTypePin, *CallTaskTypePin);
    }
    else
    {
        CallTaskTypePin->DefaultObject = SpawnClass;
    }

    // Copy the World Context pin if necessary.
    if (SpawnWorldContextPin)
    {
        CompilerContext.MovePinLinksToIntermediate(*SpawnWorldContextPin, *CallWorldContextPin);
    }

    // Copy the "Result" pin's type from the spawned node to the actual node (this ensures
    // the pin has the correct subclass type for the result).
    CallResult->PinType = SpawnNodeResult->PinType;

    // Copy the "Result" pin to the node.
    CompilerContext.MovePinLinksToIntermediate(*SpawnNodeResult, *CallResult);

    // Create the assignment nodes that set the values of the "Expose on Spawn" variables
    // after the object is created. This returns the pin of the last assignment node, and we
    // pass in the pins and node that the assignment needs to be wired up to.
    UEdGraphPin *LastThen = this->GenerateAssignmentNodes(
        CompilerContext,
        SourceGraph,
        CallTaskNode,
        CallThenPin,
        this,
        CallResult,
        GetClassToSpawn());

    // Copy the "Enqueued" pin to the last assignment node's "Exec Out".
    CompilerContext.CopyPinLinksToIntermediate(*SpawnNodeThenPin, *LastThen);

    // Break all of the node links (because we've wired them up to the underlying
    // node that we created internally).
    this->BreakAllNodeLinks();
}

UEdGraphPin *UBPNode_EnqueueGameManagerTask::GenerateAssignmentNodes(
    class FKismetCompilerContext &CompilerContext,
    UEdGraph *SourceGraph,
    UK2Node_CallFunction *CallBeginSpawnNode,
    UEdGraphPin *ThenPin,
    UEdGraphNode *SpawnNode,
    UEdGraphPin *CallBeginResult,
    const UClass *ForClass)
{
    static const FName ObjectParamName(TEXT("Object"));
    static const FName ValueParamName(TEXT("Value"));
    static const FName PropertyNameParamName(TEXT("PropertyName"));

    const UEdGraphSchema_K2 *Schema = CompilerContext.GetSchema();
    UEdGraphPin *LastThen = ThenPin;

    // Create 'set var by name' nodes and hook them up
    for (int32 PinIdx = 0; PinIdx < SpawnNode->Pins.Num(); PinIdx++)
    {
        // Only create 'set param by name' node if this pin is linked to something
        UEdGraphPin *OrgPin = SpawnNode->Pins[PinIdx];
        const bool bHasDefaultValue =
            !OrgPin->DefaultValue.IsEmpty() || !OrgPin->DefaultTextValue.IsEmpty() || OrgPin->DefaultObject;
        if (NULL == CallBeginSpawnNode->FindPin(OrgPin->PinName) && (OrgPin->LinkedTo.Num() > 0 || bHasDefaultValue))
        {
            if (OrgPin->LinkedTo.Num() == 0)
            {
                FProperty *Property = FindFProperty<FProperty>(ForClass, OrgPin->PinName);
                // NULL property indicates that this pin was part of the original node, not the
                // class we're assigning to:
                if (!Property)
                {
                    continue;
                }

                // We don't want to generate an assignment node unless the default value
                // differs from the value in the CDO:
                FString DefaultValueAsString;

                if (FBlueprintCompilationManager::GetDefaultValue(ForClass, Property, DefaultValueAsString))
                {
                    if (DefaultValueAsString == OrgPin->GetDefaultAsString())
                    {
                        continue;
                    }
                }
                else if (ForClass->ClassDefaultObject)
                {
                    FBlueprintEditorUtils::PropertyValueToString(
                        Property,
                        (uint8 *)ForClass->ClassDefaultObject,
                        DefaultValueAsString);

                    if (DefaultValueAsString == OrgPin->GetDefaultAsString())
                    {
                        continue;
                    }
                }
            }

            UFunction *SetByNameFunction = Schema->FindSetVariableByNameFunction(OrgPin->PinType);
            if (SetByNameFunction)
            {
                UK2Node_CallFunction *SetVarNode = nullptr;
                if (OrgPin->PinType.IsArray())
                {
                    SetVarNode =
                        CompilerContext.SpawnIntermediateNode<UK2Node_CallArrayFunction>(SpawnNode, SourceGraph);
                }
                else
                {
                    SetVarNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(SpawnNode, SourceGraph);
                }
                SetVarNode->SetFromFunction(SetByNameFunction);
                SetVarNode->AllocateDefaultPins();

                // Connect this node into the exec chain
                Schema->TryCreateConnection(LastThen, SetVarNode->GetExecPin());
                LastThen = SetVarNode->GetThenPin();

                // Connect the new actor to the 'object' pin
                UEdGraphPin *ObjectPin = SetVarNode->FindPinChecked(ObjectParamName);
                CallBeginResult->MakeLinkTo(ObjectPin);

                // Fill in literal for 'property name' pin - name of pin is property name
                UEdGraphPin *PropertyNamePin = SetVarNode->FindPinChecked(PropertyNameParamName);
                PropertyNamePin->DefaultValue = OrgPin->PinName.ToString();

                UEdGraphPin *ValuePin = SetVarNode->FindPinChecked(ValueParamName);
                if (OrgPin->LinkedTo.Num() == 0 && OrgPin->DefaultValue != FString() &&
                    OrgPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Byte &&
                    OrgPin->PinType.PinSubCategoryObject.IsValid() &&
                    OrgPin->PinType.PinSubCategoryObject->IsA<UEnum>())
                {
                    // Pin is an enum, we need to alias the enum value to an int:
                    UK2Node_EnumLiteral *EnumLiteralNode =
                        CompilerContext.SpawnIntermediateNode<UK2Node_EnumLiteral>(SpawnNode, SourceGraph);
                    EnumLiteralNode->Enum = CastChecked<UEnum>(OrgPin->PinType.PinSubCategoryObject.Get());
                    EnumLiteralNode->AllocateDefaultPins();
                    EnumLiteralNode->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue)->MakeLinkTo(ValuePin);

                    UEdGraphPin *InPin = EnumLiteralNode->FindPinChecked(UK2Node_EnumLiteral::GetEnumInputPinName());
                    check(InPin);
                    InPin->DefaultValue = OrgPin->DefaultValue;
                }
                else
                {
                    // For non-array struct pins that are not linked, transfer the pin type so that the node will expand
                    // an auto-ref that will assign the value by-ref.
                    if (OrgPin->PinType.IsArray() == false &&
                        OrgPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct && OrgPin->LinkedTo.Num() == 0)
                    {
                        ValuePin->PinType.PinCategory = OrgPin->PinType.PinCategory;
                        ValuePin->PinType.PinSubCategory = OrgPin->PinType.PinSubCategory;
                        ValuePin->PinType.PinSubCategoryObject = OrgPin->PinType.PinSubCategoryObject;
                        CompilerContext.MovePinLinksToIntermediate(*OrgPin, *ValuePin);
                    }
                    else
                    {
                        CompilerContext.MovePinLinksToIntermediate(*OrgPin, *ValuePin);
                        SetVarNode->PinConnectionListChanged(ValuePin);
                    }
                }
            }
        }
    }

    return LastThen;
}

#undef LOCTEXT_NAMESPACE