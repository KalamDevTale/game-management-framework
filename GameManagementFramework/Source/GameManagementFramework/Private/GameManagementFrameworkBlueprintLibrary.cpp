// Copyright June Rhodes. All Rights Reserved.

#include "GameManagementFrameworkBlueprintLibrary.h"

#include "Engine/World.h"
#include "GameManagementFrameworkModule.h"
#include "GameManagementSubsystem.h"

UGameManager *UGameManagementFrameworkBlueprintLibrary::GetGameManager(UObject *ContextObject, FName ManagerName)
{
    auto Subsystem = ContextObject->GetWorld()->GetGameInstance()->GetSubsystem<UGameManagementSubsystem>();

    if (!Subsystem->GameManagerInstances.Contains(ManagerName))
    {
        UE_LOG(
            LogGameManagementFramework,
            Error,
            TEXT("No game manager registered for manager name %s"),
            *ManagerName.ToString());
        return nullptr;
    }

    return Subsystem->GameManagerInstances[ManagerName];
}