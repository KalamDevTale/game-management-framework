// Copyright June Rhodes. All Rights Reserved.

#include "GameManagerLoadingScreen.h"
#include "Engine/Engine.h"

void UGameManagerLoadingScreen::FinishDestroy()
{
    Super::FinishDestroy();
}

bool UGameManagerLoadingScreen::IsTickable() const
{
    if (HasAnyFlags(RF_ClassDefaultObject))
    {
        return false;
    }

    return true;
}

UWorld *UGameManagerLoadingScreen::GetWorld() const
{
    if (HasAnyFlags(RF_ClassDefaultObject))
    {
        return nullptr;
    }

    UGameInstance *GameInstance = Cast<UGameInstance>(GetOuter());

    if (GameInstance == nullptr)
    {
        return nullptr;
    }

    struct FWorldContext *WorldContext = GameInstance->GetWorldContext();
    if (WorldContext != nullptr)
    {
        return WorldContext->World();
    }

    return nullptr;
}

FTimerManager *UGameManagerLoadingScreen::GetTimerManager() const
{
    UGameInstance *GameInstance = Cast<UGameInstance>(GetOuter());

    if (GameInstance == nullptr)
    {
        return nullptr;
    }

    return GameInstance->TimerManager;
}

FLatentActionManager *UGameManagerLoadingScreen::GetLatentActionManager() const
{
    UGameInstance *GameInstance = Cast<UGameInstance>(GetOuter());

    if (GameInstance == nullptr)
    {
        return nullptr;
    }

    return GameInstance->LatentActionManager;
}