// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "Containers/Ticker.h"
#include "CoreMinimal.h"
#include "GameManagementFrameworkConfig.h"
#include "Modules/ModuleManager.h"
#include "UObject/Object.h"
#include "UObject/UnrealType.h"

class FGameManagementFrameworkEditorModule : public IModuleInterface
{
private:
    FTSTicker::FDelegateHandle RefreshHandle;
    FDelegateHandle OnSettingsChanged;
    TSoftObjectPtr<UGameManagementFrameworkConfig> Config;

public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    static void OnSettingChanged(UObject *, struct FPropertyChangedEvent &Property);
    static bool OnRefreshDelayElapsed(float);
};