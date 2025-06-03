// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "UObject/Object.h"

#include "GameManagementFrameworkConfig.generated.h"

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Game Management Framework"))
class GAMEMANAGEMENTFRAMEWORK_API UGameManagementFrameworkConfig : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    /**
     * Configures the implementation of the loading screen.
     */
    UPROPERTY(
        Config,
        NoClear,
        EditAnywhere,
        Category = "Loading Screen",
        meta = (MetaClass = "GameManagerLoadingScreen", DisplayName = "Loading Screen"))
    FSoftClassPath LoadingScreenClassName;

    UPROPERTY(
        Config,
        EditAnywhere,
        Category = "Game Managers",
        meta = (MetaClass = "GameManager", DisplayName = "Game Managers"))
    TMap<FName, FSoftClassPath> GameManagers;
};