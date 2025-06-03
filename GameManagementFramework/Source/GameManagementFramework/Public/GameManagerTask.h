// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LatentActionManager.h"
#include "GameManagerTaskReadyState.h"
#include "Runtime/Engine/Public/Tickable.h"
#include "TimerManager.h"
#include "UObject/ObjectMacros.h"

#include "GameManagerTask.generated.h"

/**
 * The base class for game manager tasks, which are scheduled, latent tasks that
 * have their results automatically cached at the game manager level.
 */
UCLASS(Transient, BlueprintType, Blueprintable, Within = GameManager)
class GAMEMANAGEMENTFRAMEWORK_API UGameManagerTask : public UObject, public FTickableGameObject
{
    GENERATED_BODY()

public:
    /**
     * If enabled, the Tick event for the task will fire. This is turned off by default for performance reasons (most
     * tasks shouldn't need it).
     */
    UPROPERTY(EditDefaultsOnly, Category = "Task Behaviour")
    bool EnableTick;

    //~ Begin UObject Interface
    virtual class UWorld *GetWorld() const override;
    //~ End UObject Interface

    //~ Begin FTickableGameObject Interface
    UFUNCTION(BlueprintImplementableEvent, Category = "Task Behaviour")
    /** Tick event. You MUST turn on "Enable Tick" in the Class Defaults for this event to fire. */
    void Tick(float DeltaTime) override;
    bool IsTickable() const override;
    ETickableTickType GetTickableTickType() const override;
    TStatId GetStatId() const override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(GameManagementFramework, STATGROUP_Tickables);
    };
    //~ End FTickableGameObject Interface

    UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = "ResultBranches", BlueprintInternalUseOnly = "true"))
    virtual void Activate(EGameManagerTaskReadyState &ResultBranches);

    /**
     * Called when the task is scheduled by a Game Manager. Variables marked "Expose on Spawn" will be the
     * input variables required when the task is scheduled. The event graph from this event must at some
     * point call "Mark Task as Complete".
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Task Behaviour", meta = (DisplayName = "Scheduled"))
    void Scheduled();

    UPROPERTY()
    bool Ready;

    /**
     * Available to be called by loading screens when this task is running in the task queue.
     */
    UFUNCTION(
        BlueprintImplementableEvent,
        BlueprintCallable,
        Category = "Loading Screens",
        meta = (DisplayName = "Get Loading Screen Text"))
    FText GetLoadingScreenText();

protected:
    UFUNCTION(BlueprintCallable, Category = "Task Behaviour", meta = (DisplayName = "Mark Task as Complete"))
    void Completed();
};