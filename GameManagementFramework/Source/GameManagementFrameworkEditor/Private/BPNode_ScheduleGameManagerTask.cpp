// Copyright June Rhodes. All Rights Reserved.

#include "BPNode_ScheduleGameManagerTask.h"

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

struct FBPNode_ScheduleGameManagerTaskHelper
{
    static FName WorldContextPinName;
    static FName ClassPinName;
    static FName OuterPinName;
    static FName TaskIdPinName;
    static FName ReadyPinName;
    static FName NotReadyPinName;
    static FName CreatedNotReadyPinName;
};

FName FBPNode_ScheduleGameManagerTaskHelper::WorldContextPinName(TEXT("WorldContextObject"));
FName FBPNode_ScheduleGameManagerTaskHelper::ClassPinName(TEXT("Class"));
FName FBPNode_ScheduleGameManagerTaskHelper::OuterPinName(TEXT("Outer"));
FName FBPNode_ScheduleGameManagerTaskHelper::TaskIdPinName(TEXT("Task Id"));
FName FBPNode_ScheduleGameManagerTaskHelper::ReadyPinName(TEXT("Ready"));
FName FBPNode_ScheduleGameManagerTaskHelper::NotReadyPinName(TEXT("NotReady"));
FName FBPNode_ScheduleGameManagerTaskHelper::CreatedNotReadyPinName(TEXT("CreatedNotReady"));

UBPNode_ScheduleGameManagerTask::UBPNode_ScheduleGameManagerTask(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    NodeTooltip = LOCTEXT("NodeTooltip", "Schedules a Game Manager Task");
}

void UBPNode_ScheduleGameManagerTask::AllocateDefaultPins()
{
    // Add execution pins
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
    CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, FBPNode_ScheduleGameManagerTaskHelper::ReadyPinName);
    CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, FBPNode_ScheduleGameManagerTaskHelper::NotReadyPinName);
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
            FBPNode_ScheduleGameManagerTaskHelper::WorldContextPinName);
    }

    // Add blueprint pin
    UEdGraphPin *ClassPin = CreatePin(
        EGPD_Input,
        UEdGraphSchema_K2::PC_Class,
        GetClassPinBaseClass(),
        FBPNode_ScheduleGameManagerTaskHelper::ClassPinName);

    // Result pin
    UEdGraphPin *ResultPin =
        CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Object, GetClassPinBaseClass(), UEdGraphSchema_K2::PN_ReturnValue);

    if (UseOuter())
    {
        UEdGraphPin *OuterPin = CreatePin(
            EGPD_Input,
            UEdGraphSchema_K2::PC_Object,
            UObject::StaticClass(),
            FBPNode_ScheduleGameManagerTaskHelper::OuterPinName);
    }

    const UEdGraphSchema_K2 *K2Schema = GetDefault<UEdGraphSchema_K2>();

    // TaskId pin
    UEdGraphPin *TaskIdPin = CreatePin(
        EGPD_Input,
        UEdGraphSchema_K2::PC_String,
        TEXT(""),
        FBPNode_ScheduleGameManagerTaskHelper::TaskIdPinName);
    SetPinToolTip(
        *TaskIdPin,
        LOCTEXT(
            "TaskIdPinDescription",
            "The unique ID of this task in the Game Manager; scheduling with the same task ID will return the existing "
            "scheduled instance."));
}

FText UBPNode_ScheduleGameManagerTask::GetBaseNodeTitle() const
{
    return LOCTEXT("ScheduleGameManagerTask_BaseTitle", "Schedule Game Manager Task");
}

bool UBPNode_ScheduleGameManagerTask::IsCompatibleWithGraph(const UEdGraph *TargetGraph) const
{
    UBlueprint *Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(TargetGraph);
    return Super::IsCompatibleWithGraph(TargetGraph);
}

FText UBPNode_ScheduleGameManagerTask::GetNodeTitleFormat() const
{
    return LOCTEXT("ScheduleGameManagerTask", "Schedule {ClassName} Task");
}

UClass *UBPNode_ScheduleGameManagerTask::GetClassPinBaseClass() const
{
    return UGameManagerTask::StaticClass();
}

FText UBPNode_ScheduleGameManagerTask::GetMenuCategory() const
{
    return FText::FromString("Game Management Framework");
}

void UBPNode_ScheduleGameManagerTask::GetMenuActions(FBlueprintActionDatabaseRegistrar &ActionRegistrar) const
{
    UClass *ActionKey = GetClass();

    if (ActionRegistrar.IsOpenForRegistration(ActionKey))
    {
        UBlueprintNodeSpawner *NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
        check(NodeSpawner != nullptr);

        ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
    }
}

UEdGraphPin *UBPNode_ScheduleGameManagerTask::GetTaskIdPin() const
{
    UEdGraphPin *Pin = FindPin(FBPNode_ScheduleGameManagerTaskHelper::TaskIdPinName);
    check(Pin == NULL || Pin->Direction == EGPD_Input);
    return Pin;
}

bool UBPNode_ScheduleGameManagerTask::IsSpawnVarPin(UEdGraphPin *Pin) const
{
    return (
        Super::IsSpawnVarPin(Pin) && Pin->PinName != FBPNode_ScheduleGameManagerTaskHelper::TaskIdPinName &&
        !(Pin->PinName == UEdGraphSchema_K2::PN_Self && Pin->Direction == EGPD_Input) &&
        !(Pin->PinName == FBPNode_ScheduleGameManagerTaskHelper::ReadyPinName && Pin->Direction == EGPD_Output) &&
        !(Pin->PinName == FBPNode_ScheduleGameManagerTaskHelper::NotReadyPinName && Pin->Direction == EGPD_Output));
}

void UBPNode_ScheduleGameManagerTask::ExpandNode(class FKismetCompilerContext &CompilerContext, UEdGraph *SourceGraph)
{
    Super::ExpandNode(CompilerContext, SourceGraph);

    static FName ScheduleTask_FunctionName = GET_FUNCTION_NAME_CHECKED(UGameManager, ScheduleTask);
    static FName ActivateTask_FunctionName = GET_FUNCTION_NAME_CHECKED(UGameManagerTask, Activate);

    static FString WorldContextObject_ParamName = FString(TEXT("WorldContextObject"));
    static FString TaskType_ParamName = FString(TEXT("TaskType"));
    static FString TaskId_ParamName = FString(TEXT("TaskId"));
    static FString Ready_ParamName = FString(TEXT("Ready"));
    static FString NotReady_ParamName = FString(TEXT("NotReady"));
    static FString CreatedNotReady_ParamName = FString(TEXT("CreatedNotReady"));

    UEdGraphPin *SpawnNodeExec = this->GetExecPin();
    UEdGraphPin *SpawnWorldContextPin = this->GetWorldContextPin();
    UEdGraphPin *SpawnTaskTypePin = this->GetClassPin();
    UEdGraphPin *SpawnTaskIdPin = this->GetTaskIdPin();
    UEdGraphPin *SpawnNodeReadyPin = this->FindPinChecked(Ready_ParamName, EGPD_Output);
    UEdGraphPin *SpawnNodeNotReadyPin = this->FindPinChecked(NotReady_ParamName, EGPD_Output);
    UEdGraphPin *SpawnNodeResult = this->GetResultPin();
    UEdGraphPin *SpawnSelfPin = this->FindPinChecked(UEdGraphSchema_K2::PN_Self, EGPD_Input);

    UClass *SpawnClass = (SpawnTaskTypePin != NULL) ? Cast<UClass>(SpawnTaskTypePin->DefaultObject) : NULL;
    if ((0 == SpawnTaskTypePin->LinkedTo.Num()) && (NULL == SpawnClass))
    {
        CompilerContext.MessageLog.Error(
            *LOCTEXT("ScheduleGameManagerTaskMissingClass_Error", "Spawn node @@ must have a class specified.")
                 .ToString(),
            this);
        this->BreakAllNodeLinks();
        return;
    }

    // Create the actual node that we represent (the call to the ScheduleTask function)
    UK2Node_CallFunction *CallScheduleTaskNode =
        CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    CallScheduleTaskNode->FunctionReference.SetExternalMember(ScheduleTask_FunctionName, UGameManager::StaticClass());
    CallScheduleTaskNode->AllocateDefaultPins();

    // Get references to the pins on that ScheduleTask node.
    UEdGraphPin *CallScheduleExec = CallScheduleTaskNode->GetExecPin();
    UEdGraphPin *CallScheduleSelfPin = CallScheduleTaskNode->FindPinChecked(UEdGraphSchema_K2::PN_Self, EGPD_Input);
    UEdGraphPin *CallScheduleWorldContextPin = CallScheduleTaskNode->FindPinChecked(WorldContextObject_ParamName);
    UEdGraphPin *CallScheduleTaskTypePin = CallScheduleTaskNode->FindPinChecked(TaskType_ParamName);
    UEdGraphPin *CallScheduleTaskIdPin = CallScheduleTaskNode->FindPinChecked(TaskId_ParamName);
    UEdGraphPin *CallScheduleReadyPin = CallScheduleTaskNode->FindPinChecked(Ready_ParamName, EGPD_Output);
    UEdGraphPin *CallScheduleNotReadyPin = CallScheduleTaskNode->FindPinChecked(NotReady_ParamName, EGPD_Output);
    UEdGraphPin *CallScheduleCreatedNotReadyPin =
        CallScheduleTaskNode->FindPinChecked(CreatedNotReady_ParamName, EGPD_Output);
    UEdGraphPin *CallScheduleResult = CallScheduleTaskNode->GetReturnValuePin();

    // Create the Activate call on the task, which we'll invoke after assigning all the variables.
    UK2Node_CallFunction *CallActivateTaskNode =
        CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    CallActivateTaskNode->FunctionReference.SetExternalMember(ActivateTask_FunctionName, SpawnClass);
    CallActivateTaskNode->AllocateDefaultPins();

    // Get references to the pins on that Activate node.
    UEdGraphPin *CallActivateExec = CallActivateTaskNode->GetExecPin();
    UEdGraphPin *CallActivateSelfPin = CallActivateTaskNode->FindPinChecked(UEdGraphSchema_K2::PN_Self, EGPD_Input);
    UEdGraphPin *CallActivateReadyPin = CallActivateTaskNode->FindPinChecked(Ready_ParamName, EGPD_Output);
    UEdGraphPin *CallActivateNotReadyPin = CallActivateTaskNode->FindPinChecked(NotReady_ParamName, EGPD_Output);

    // Map our "Exec In" pin to the ScheduleTask node.
    CompilerContext.MovePinLinksToIntermediate(*SpawnNodeExec, *CallScheduleExec);

    // Map out "Self" pin to the ScheduleTask node.
    CompilerContext.MovePinLinksToIntermediate(*SpawnSelfPin, *CallScheduleSelfPin);

    // Map our "Task Type" pin to the ScheduleTask node (or set a default).
    if (SpawnTaskTypePin->LinkedTo.Num() > 0)
    {
        CompilerContext.MovePinLinksToIntermediate(*SpawnTaskTypePin, *CallScheduleTaskTypePin);
    }
    else
    {
        CallScheduleTaskTypePin->DefaultObject = SpawnClass;
    }

    // Copy the "Task Id" pin to the ScheduleTask node.
    CompilerContext.MovePinLinksToIntermediate(*SpawnTaskIdPin, *CallScheduleTaskIdPin);

    // Copy the World Context pin if necessary.
    if (SpawnWorldContextPin)
    {
        CompilerContext.MovePinLinksToIntermediate(*SpawnWorldContextPin, *CallScheduleWorldContextPin);
    }

    // Copy the "Result" pin's type from the spawned node to the actual node (this ensures
    // the pin has the correct subclass type for the result).
    CallScheduleResult->PinType = SpawnNodeResult->PinType;

    // Copy the "Result" pin to the ScheduleTask node.
    CompilerContext.MovePinLinksToIntermediate(*SpawnNodeResult, *CallScheduleResult);

    // Create the assignment nodes that set the values of the "Expose on Spawn" variables
    // after the object is created. This returns the pin of the last assignment node, and we
    // pass in the pins and node that the assignment needs to be wired up to.
    UEdGraphPin *LastThen = this->GenerateAssignmentNodes(
        CompilerContext,
        SourceGraph,
        CallScheduleTaskNode,
        CallScheduleCreatedNotReadyPin,
        this,
        CallScheduleResult,
        GetClassToSpawn());

    // Set the Activate node's "Self" value to the "Result" pin of the ScheduleTask node.
    CallActivateSelfPin->MakeLinkTo(CallScheduleResult);

    // Make the last "Then" from the assignment list connect to the "Exec" pin of the Activate node.
    LastThen->MakeLinkTo(CallActivateExec);

    // Copy the "Not Ready" pin to the Activate node.
    CompilerContext.CopyPinLinksToIntermediate(*SpawnNodeNotReadyPin, *CallActivateNotReadyPin);

    // Copy the "Ready" pin to the Activate node.
    CompilerContext.CopyPinLinksToIntermediate(*SpawnNodeReadyPin, *CallActivateReadyPin);

    // Copy the "Not Ready" pin to the ScheduleTask node.
    CompilerContext.MovePinLinksToIntermediate(*SpawnNodeNotReadyPin, *CallScheduleNotReadyPin);

    // Copy the "Ready" pin to the ScheduleTask node.
    CompilerContext.MovePinLinksToIntermediate(*SpawnNodeReadyPin, *CallScheduleReadyPin);

    // Break all of the node links (because we've wired them up to the underlying
    // "Schedule Task" node that we created internally).
    this->BreakAllNodeLinks();
}

UEdGraphPin *UBPNode_ScheduleGameManagerTask::GenerateAssignmentNodes(
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