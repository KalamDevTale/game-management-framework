// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "BlueprintActionDatabaseRegistrar.h"
#include "EdGraph/EdGraph.h"
#include "K2Node.h"
#include "K2Node_CallFunction.h"
#include "K2Node_ConstructObjectFromClass.h"

#include "BPNode_ScheduleGameManagerTask.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GAMEMANAGEMENTFRAMEWORKEDITOR_API UBPNode_ScheduleGameManagerTask : public UK2Node_ConstructObjectFromClass
{
    GENERATED_UCLASS_BODY()

    // Begin UEdGraphNode interface.
    virtual void AllocateDefaultPins() override;
    virtual bool IsCompatibleWithGraph(const UEdGraph *TargetGraph) const override;
    virtual void ExpandNode(class FKismetCompilerContext &CompilerContext, UEdGraph *SourceGraph) override;
    // End UEdGraphNode interface.

    // Begin UK2Node interface
    void GetMenuActions(FBlueprintActionDatabaseRegistrar &ActionRegistrar) const;
    virtual FText GetMenuCategory() const override;
    // End UK2Node interface.

    UEdGraphPin *GetTaskIdPin() const;

protected:
    /** Gets the default node title when no class is selected */
    virtual FText GetBaseNodeTitle() const;
    /** Gets the node title when a class has been selected. */
    virtual FText GetNodeTitleFormat() const;
    /** Gets base class to use for the 'class' pin.  UObject by default. */
    virtual UClass *GetClassPinBaseClass() const;

    virtual bool IsSpawnVarPin(UEdGraphPin *Pin) const override;

private:
    UEdGraphPin *GenerateAssignmentNodes(
        class FKismetCompilerContext &CompilerContext,
        UEdGraph *SourceGraph,
        UK2Node_CallFunction *CallBeginSpawnNode,
        UEdGraphPin *ThenPin,
        UEdGraphNode *SpawnNode,
        UEdGraphPin *CallBeginResult,
        const UClass *ForClass);
};