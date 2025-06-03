// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/LatentActionManager.h"
#include "GameManagerTask.h"
#include "Runtime/Engine/Public/Tickable.h"
#include "TimerManager.h"
#include "UObject/ObjectMacros.h"

#include "GameManagerLoadingScreen.generated.h"

/**
 * The base class for a loading screen, which will be instantiated by the Game Management Framework, and have it's
 * events called when the loading screen should be shown, updated or hidden.
 */
UCLASS(Transient, BlueprintType, Blueprintable, Abstract)
class GAMEMANAGEMENTFRAMEWORK_API UGameManagerLoadingScreen : public UObject, public FTickableGameObject
{
    GENERATED_BODY()

public:
    //~ Begin UObject Interface
    virtual class UWorld *GetWorld() const override;
    virtual void FinishDestroy() override;
    //~ End UObject Interface

    FTimerManager *GetTimerManager() const;

    FLatentActionManager *GetLatentActionManager() const;

    //~ Begin FTickableGameObject Interface
    UFUNCTION(BlueprintImplementableEvent)
    void Tick(float DeltaTime) override;
    bool IsTickable() const override;
    TStatId GetStatId() const override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(GameManagementFramework, STATGROUP_Tickables);
    };
    //~ End FTickableGameObject Interface

    UFUNCTION(BlueprintImplementableEvent, Category = "Game Management Framework")
    void ShowLoadingScreen(UGameManagerTask *InitialTask);

    UFUNCTION(BlueprintImplementableEvent, Category = "Game Management Framework")
    void HideLoadingScreen();

    UFUNCTION(BlueprintImplementableEvent, Category = "Game Management Framework")
    void UpdateLoadingScreenStatus(UGameManagerTask *NewTask);
};