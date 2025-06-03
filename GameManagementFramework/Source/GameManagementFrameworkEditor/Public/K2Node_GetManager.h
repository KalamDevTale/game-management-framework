// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "GameManager.h"
#include "K2Node.h"

#include "K2Node_GetManager.generated.h"

UCLASS()
class GAMEMANAGEMENTFRAMEWORKEDITOR_API UK2Node_GetManager : public UK2Node
{
    GENERATED_BODY()

private:
    virtual TSubclassOf<UGameManager> GetManagerClass() const;

public:
    void Initialize(FName InName);

    virtual void AllocateDefaultPins() override;
    virtual bool IsCompatibleWithGraph(const UEdGraph *TargetGraph) const override;
    virtual FSlateIcon GetIconAndTint(FLinearColor &OutColor) const override;
    virtual FLinearColor GetNodeTitleColor() const override;
    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    virtual void ExpandNode(FKismetCompilerContext &CompilerContext, UEdGraph *SourceGraph) override;

    virtual FText GetTooltipText() const override;

    virtual bool IsNodePure() const
    {
        return true;
    }

    virtual bool IsNodeSafeToIgnore() const override
    {
        return true;
    }

    virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin *> &OldPins) override;
    virtual class FNodeHandlingFunctor *CreateNodeHandler(class FKismetCompilerContext &CompilerContext) const override;
    virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar &ActionRegistrar) const override;
    // virtual void GetNodeAttributes(TArray<TKeyValuePair<FString, FString>> &OutNodeAttributes) const override;
    virtual FText GetMenuCategory() const override;

    /** Get the blueprint input pin */
    UEdGraphPin *GetManagerNamePin(const TArray<UEdGraphPin *> *InPinsToSearch = nullptr) const;
    /** Get the world context input pin, can return NULL */
    UEdGraphPin *GetWorldContextPin() const;
    /** Get the result output pin */
    UEdGraphPin *GetResultPin() const;

    virtual bool ShouldDrawCompact() const override
    {
        return true;
    }

protected:
    UPROPERTY()
    FName Name;
};