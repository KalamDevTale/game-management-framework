// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

GAMEMANAGEMENTFRAMEWORK_API DECLARE_LOG_CATEGORY_EXTERN(LogGameManagementFramework, All, All);

class GAMEMANAGEMENTFRAMEWORK_API FGameManagementFrameworkModule : public IModuleInterface
{
#if WITH_EDITOR
    virtual void StartupModule() override
    {
        FModuleManager::Get().LoadModule(TEXT("GameManagementFrameworkEditor"));
    }
#endif // WITH_EDITOR
};