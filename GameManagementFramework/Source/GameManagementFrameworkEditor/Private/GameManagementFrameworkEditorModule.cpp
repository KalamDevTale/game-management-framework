// Copyright June Rhodes. All Rights Reserved.

#include "GameManagementFrameworkEditorModule.h"

#include "BlueprintActionDatabase.h"
#include "UObject/Object.h"
#include "UObject/UObjectGlobals.h"

#define LOCTEXT_NAMESPACE "FGameManagementFrameworkEditorModule"

DECLARE_LOG_CATEGORY_EXTERN(LogRedpointGameManagementFrameworkEditor, All, All);
DEFINE_LOG_CATEGORY(LogRedpointGameManagementFrameworkEditor);

void FGameManagementFrameworkEditorModule::StartupModule()
{
    this->RefreshHandle.Reset();

    auto *CDO = GetMutableDefault<UGameManagementFrameworkConfig>();
    if (IsValid(CDO))
    {
        this->Config = CDO;
        this->OnSettingsChanged =
            this->Config->OnSettingChanged().AddStatic(&FGameManagementFrameworkEditorModule::OnSettingChanged);
    }
    else
    {
        UE_LOG(
            LogRedpointGameManagementFrameworkEditor,
            Error,
            TEXT("Unable to get UGameManagementFrameworkConfig object on startup - blueprint actions will not be "
                 "refreshed when you change the configuration!"));
    }
}

void FGameManagementFrameworkEditorModule::ShutdownModule()
{
    if (this->Config.IsValid())
    {
        this->Config->OnSettingChanged().Remove(this->OnSettingsChanged);
    }
}

void FGameManagementFrameworkEditorModule::OnSettingChanged(UObject *, struct FPropertyChangedEvent &Property)
{
    if (!Property.GetPropertyName().IsEqual(FName(TEXT("GameManagers"))))
    {
        return;
    }

    auto &Module = FModuleManager::GetModuleChecked<FGameManagementFrameworkEditorModule>(
        FName(TEXT("GameManagementFrameworkEditor")));

    UE_LOG(
        LogRedpointGameManagementFrameworkEditor,
        Verbose,
        TEXT("Scheduling a refresh of the blueprint action database in 2.5 seconds as the configuration of game "
             "managers has been changed in Project Settings."));

    if (Module.RefreshHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(Module.RefreshHandle);
    }

    Module.RefreshHandle = FTSTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateStatic(&FGameManagementFrameworkEditorModule::OnRefreshDelayElapsed),
        2.5f);
}

bool FGameManagementFrameworkEditorModule::OnRefreshDelayElapsed(float)
{
    auto &Module = FModuleManager::GetModuleChecked<FGameManagementFrameworkEditorModule>(
        FName(TEXT("GameManagementFrameworkEditor")));

    UE_LOG(
        LogRedpointGameManagementFrameworkEditor,
        Verbose,
        TEXT("Refreshing blueprint action database as the configuration of game managers has been "
             "changed in Project Settings."));
    FBlueprintActionDatabase::Get().RefreshAll();

    Module.RefreshHandle.Reset();
    return false;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGameManagementFrameworkEditorModule, GameManagementFrameworkEditor)