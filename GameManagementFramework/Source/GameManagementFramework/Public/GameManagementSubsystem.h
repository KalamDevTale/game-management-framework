// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "Containers/Ticker.h"
#include "CoreMinimal.h"
#include "GameManagementFrameworkBlueprintLibrary.h"
#include "GameManagerLoadingScreen.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "GameManagementSubsystem.generated.h"

#if defined(UE_5_0_OR_LATER)
typedef FTSTicker FUTicker;
typedef FTSTicker::FDelegateHandle FUTickerDelegateHandle;
#else
typedef FTicker FUTicker;
typedef FDelegateHandle FUTickerDelegateHandle;
#endif // #if defined(UE_5_0_OR_LATER)

UCLASS()
class GAMEMANAGEMENTFRAMEWORK_API UGameManagementSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

    friend class UGameManagementFrameworkBlueprintLibrary;

private:
    UPROPERTY()
    UGameManagerLoadingScreen *LoadingScreen;

    UPROPERTY()
    TArray<UGameManagerTask *> TaskQueue;

    UPROPERTY()
    TArray<bool> LoadingScreenQueue;

    UPROPERTY()
    bool bHasShownLoadingScreen;

    UPROPERTY()
    UGameManagerTask *ActiveTask;

    UPROPERTY()
    TMap<FName, UGameManager *> GameManagerInstances;

    bool Tick(float DeltaTime);
    FUTickerDelegateHandle TickDelegateHandle;

public:
    UGameManagerLoadingScreen *GetLoadingScreen();

    void EnqueueTask(UGameManagerTask *InTask, bool bShowLoadingScreen);

    virtual void Initialize(FSubsystemCollectionBase &Collection) override;
    virtual void Deinitialize() override;
};