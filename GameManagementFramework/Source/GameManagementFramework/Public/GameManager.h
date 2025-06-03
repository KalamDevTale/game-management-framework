// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/LatentActionManager.h"
#include "GameManagerTask.h"
#include "GameManagerTaskReadyState.h"
#include "Runtime/Engine/Public/Tickable.h"
#include "TimerManager.h"
#include "UObject/ObjectMacros.h"

#include "GameManager.generated.h"

class FGameManagerTicker : public FTickableGameObject
{
private:
    bool bHasCalledInit;
    TSoftObjectPtr<class UGameManager> GameManager;

public:
    FGameManagerTicker(TSoftObjectPtr<class UGameManager> Owner);
    void Tick(float DeltaTime) override;
    bool IsTickable() const override;
    TStatId GetStatId() const override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(GameManagementFramework, STATGROUP_Tickables);
    };
};

/**
 * The base class for game managers, which are persist objects that are capable of latent actions, similar to the game
 * instance.
 */
UCLASS(Transient, BlueprintType, Blueprintable, Within = GameInstance)
class GAMEMANAGEMENTFRAMEWORK_API UGameManager : public UObject
{
    GENERATED_BODY()

private:
    TSharedPtr<FGameManagerTicker> Ticker;

public:
    UGameManager(const FObjectInitializer &ObjectInitializer);

    UFUNCTION(BlueprintImplementableEvent)
    void Init();

    UFUNCTION(BlueprintImplementableEvent)
    void Tick(float DeltaTime);

    //~ Begin UObject Interface
    virtual class UWorld *GetWorld() const override;
    //~ End UObject Interface

    UFUNCTION(
        BlueprintCallable,
        meta =
            (WorldContext = "WorldContextObject",
             DisplayName = "Schedule Game Manager Task",
             ExpandEnumAsExecs = "ResultBranches",
             BlueprintInternalUseOnly = "true"),
        Category = "Game Management Framework")
    UGameManagerTask *ScheduleTask(
        UObject *WorldContextObject,
        const FString &TaskId,
        TSubclassOf<UGameManagerTask> TaskType,
        EGameManagerTaskReadyState &ResultBranches);

    UFUNCTION(
        BlueprintCallable,
        meta =
            (WorldContext = "WorldContextObject",
             DisplayName = "Enqueue Game Manager Task",
             BlueprintInternalUseOnly = "true"),
        Category = "Game Management Framework")
    UGameManagerTask *EnqueueTask(
        UObject *WorldContextObject,
        TSubclassOf<UGameManagerTask> TaskType,
        bool bShowLoadingScreen);

private:
    UPROPERTY()
    TMap<FString, UGameManagerTask *> Tasks;
};