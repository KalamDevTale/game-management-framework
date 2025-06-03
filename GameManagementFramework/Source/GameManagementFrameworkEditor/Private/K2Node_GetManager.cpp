// Copyright June Rhodes. All Rights Reserved.

#include "K2Node_GetManager.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EdGraphSchema_K2.h"
#include "Engine/Blueprint.h"
#include "GameManagementFrameworkBlueprintLibrary.h"
#include "GameManagementFrameworkConfig.h"
#include "GameManagementFrameworkModule.h"
#include "K2Node_CallFunction.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetCompiler.h"

TSubclassOf<UGameManager> UK2Node_GetManager::GetManagerClass() const
{
    auto Config = GetMutableDefault<UGameManagementFrameworkConfig>();
    if (Config->GameManagers.Contains(this->Name))
    {
        return Config->GameManagers[this->Name].TryLoadClass<UGameManager>();
    }

    return nullptr;
}

void UK2Node_GetManager::Initialize(FName InName)
{
    this->Name = InName;
}

void UK2Node_GetManager::AllocateDefaultPins()
{
    auto GameManagerClass = this->GetManagerClass();

    // If required add the world context pin
    if (GetBlueprint()->ParentClass->HasMetaData(FBlueprintMetadata::MD_ShowWorldContextPin))
    {
        CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UObject::StaticClass(), TEXT("WorldContext"));
    }

    // Add blueprint pin
    if (!GameManagerClass)
    {
        CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, USubsystem::StaticClass(), TEXT("Name"));
    }

    // Result pin
    CreatePin(
        EGPD_Output,
        UEdGraphSchema_K2::PC_Object,
        (GameManagerClass ? (UClass *)GameManagerClass : USubsystem::StaticClass()),
        UEdGraphSchema_K2::PN_ReturnValue);

    Super::AllocateDefaultPins();
}

bool UK2Node_GetManager::IsCompatibleWithGraph(const UEdGraph *TargetGraph) const
{
    UBlueprint *Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(TargetGraph);
    return Super::IsCompatibleWithGraph(TargetGraph) &&
           (!Blueprint || FBlueprintEditorUtils::FindUserConstructionScript(Blueprint) != TargetGraph);
}

FSlateIcon UK2Node_GetManager::GetIconAndTint(FLinearColor &OutColor) const
{
    OutColor = GetNodeTitleColor();
    static FSlateIcon Icon("EditorStyle", "Kismet.AllClasses.FunctionIcon");
    return Icon;
}

FLinearColor UK2Node_GetManager::GetNodeTitleColor() const
{
    return FLinearColor(1.f, 0.078f, 0.576f, 1.f);
}

FText UK2Node_GetManager::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    return FText::FormatNamed(
        NSLOCTEXT("K2Node", "GetManager_NodeTitleFormat", "Get {Name} Manager"),
        TEXT("Name"),
        FText::FromName(this->Name));
}

/*
void UK2Node_GetManager::GetNodeAttributes(TArray<TKeyValuePair<FString, FString>> &OutNodeAttributes) const
{
    auto GameManagerClass = this->GetManagerClass();

    const FString ClassToSpawnStr = GameManagerClass ? GameManagerClass->GetName() : TEXT("InvalidClass");
    OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("Type"), TEXT("GetSubsystems")));
    OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("Class"), GetClass()->GetName()));
    OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("Name"), GetName()));
    OutNodeAttributes.Add(TKeyValuePair<FString, FString>(TEXT("ObjectClass"), ClassToSpawnStr));
}
*/

void UK2Node_GetManager::ExpandNode(class FKismetCompilerContext &CompilerContext, UEdGraph *SourceGraph)
{
    Super::ExpandNode(CompilerContext, SourceGraph);

    static const FName WorldContextObject_ParamName(TEXT("ContextObject"));
    static const FName ManagerName_ParamName(TEXT("ManagerName"));

    UK2Node_GetManager *GetSubsystemNode = this;
    UEdGraphPin *SpawnWorldContextPin = GetSubsystemNode->GetWorldContextPin();
    UEdGraphPin *SpawnManagerNamePin = GetSubsystemNode->GetManagerNamePin();
    UEdGraphPin *SpawnNodeResult = GetSubsystemNode->GetResultPin();

    FName Get_FunctionName = GET_FUNCTION_NAME_CHECKED(UGameManagementFrameworkBlueprintLibrary, GetGameManager);

    UK2Node_CallFunction *CallGetNode =
        CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(GetSubsystemNode, SourceGraph);
    CallGetNode->FunctionReference.SetExternalMember(
        Get_FunctionName,
        UGameManagementFrameworkBlueprintLibrary::StaticClass());
    CallGetNode->AllocateDefaultPins();

    UEdGraphPin *CallCreateWorldContextPin = CallGetNode->FindPinChecked(WorldContextObject_ParamName);
    UEdGraphPin *CallCreateManagerNameTypePin = CallGetNode->FindPinChecked(ManagerName_ParamName);
    UEdGraphPin *CallCreateResult = CallGetNode->GetReturnValuePin();

    if (SpawnManagerNamePin && SpawnManagerNamePin->LinkedTo.Num() > 0)
    {
        CompilerContext.MovePinLinksToIntermediate(*SpawnManagerNamePin, *CallCreateManagerNameTypePin);
    }
    else
    {
        CallCreateManagerNameTypePin->DefaultValue = this->Name.ToString();
    }

    if (SpawnWorldContextPin)
    {
        CompilerContext.MovePinLinksToIntermediate(*SpawnWorldContextPin, *CallCreateWorldContextPin);
    }

    // Move result connection from spawn node to 'USubsystemBlueprintLibrary::Get[something]Subsystem'
    CallCreateResult->PinType = SpawnNodeResult->PinType;
    CompilerContext.MovePinLinksToIntermediate(*SpawnNodeResult, *CallCreateResult);

    //////////////////////////////////////////////////////////////////////////

    // Break any links to the expanded node
    GetSubsystemNode->BreakAllNodeLinks();
}

FText UK2Node_GetManager::GetTooltipText() const
{
    return FText::FormatNamed(
        NSLOCTEXT("K2Node", "GetManager_NodeTooltipFormat", "Get {Name} Manager"),
        TEXT("Name"),
        FText::FromName(this->Name));
}

void UK2Node_GetManager::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin *> &OldPins)
{
    auto GameManagerClass = this->GetManagerClass();

    if (!GameManagerClass)
    {
        if (auto NameNode = GetManagerNamePin(&OldPins))
        {
            this->Name = FName(NameNode->DefaultValue);
            GameManagerClass = this->GetManagerClass();
        }
    }

    AllocateDefaultPins();

    if (GameManagerClass)
    {
        UEdGraphPin *ResultPin = GetResultPin();
        ResultPin->PinType.PinSubCategoryObject = *GameManagerClass;
    }
}

class FNodeHandlingFunctor *UK2Node_GetManager::CreateNodeHandler(class FKismetCompilerContext &CompilerContext) const
{
    return new FNodeHandlingFunctor(CompilerContext);
}

void UK2Node_GetManager::GetMenuActions(FBlueprintActionDatabaseRegistrar &ActionRegistrar) const
{
    auto Config = GetMutableDefault<UGameManagementFrameworkConfig>();

    auto CustomizeCallback = [](UEdGraphNode *Node, bool bIsTemplateNode, TPair<FName, FSoftClassPath> Pair) {
        auto TypedNode = CastChecked<UK2Node_GetManager>(Node);
        TypedNode->Initialize(Pair.Key);
    };

    UClass *ActionKey = GetClass();
    if (ActionRegistrar.IsOpenForRegistration(ActionKey))
    {
        for (auto Entry : Config->GameManagers)
        {
            UBlueprintNodeSpawner *Spawner = UBlueprintNodeSpawner::Create(ActionKey);
            check(Spawner);

            auto ClassRef = Entry.Value.TryLoadClass<UGameManager>();
            if (ClassRef == nullptr)
            {
                UE_LOG(
                    LogGameManagementFramework,
                    Warning,
                    TEXT("Missing game manager class in game managers config!"));
                continue;
            }

            Spawner->CustomizeNodeDelegate =
                UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(CustomizeCallback, Entry);
            ActionRegistrar.AddBlueprintAction(ActionKey, Spawner);
        }
    }
}

FText UK2Node_GetManager::GetMenuCategory() const
{
    return NSLOCTEXT("GameManagementFramework", "GetManager_MenuCategory", "Game Management Framework");
}

UEdGraphPin *UK2Node_GetManager::GetWorldContextPin() const
{
    UEdGraphPin *Pin = FindPin(TEXT("WorldContext"));
    check(Pin == NULL || Pin->Direction == EGPD_Input);
    return Pin;
}

UEdGraphPin *UK2Node_GetManager::GetResultPin() const
{
    UEdGraphPin *Pin = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue);
    check(Pin->Direction == EGPD_Output);
    return Pin;
}

UEdGraphPin *UK2Node_GetManager::GetManagerNamePin(const TArray<UEdGraphPin *> *InPinsToSearch /*= nullptr */) const
{
    const TArray<UEdGraphPin *> *PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;

    UEdGraphPin *Pin = NULL;
    for (auto PinIt = PinsToSearch->CreateConstIterator(); PinIt; ++PinIt)
    {
        UEdGraphPin *TestPin = *PinIt;
        if (TestPin && TestPin->PinName == TEXT("Name"))
        {
            Pin = TestPin;
            break;
        }
    }

    return Pin;
}