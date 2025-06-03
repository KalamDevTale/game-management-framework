// Copyright June Rhodes. All Rights Reserved.

#include "GameManagementSubsystem.h"

#include "GameManagementFrameworkConfig.h"
#include "GameManagementFrameworkModule.h"
#include "Kismet/KismetSystemLibrary.h"

UGameManagerLoadingScreen *UGameManagementSubsystem::GetLoadingScreen()
{
    if (this->LoadingScreen == nullptr)
    {
        auto Config = GetMutableDefault<UGameManagementFrameworkConfig>();

        auto PlatformClass = Config->LoadingScreenClassName.TryLoadClass<UGameManagerLoadingScreen>();
        if (PlatformClass == nullptr)
        {
            UE_LOG(LogGameManagementFramework, Error, TEXT("No loading screen class set - please check your config!"));
            return nullptr;
        }

        this->LoadingScreen = NewObject<UGameManagerLoadingScreen>(this->GetOuter(), PlatformClass);
        if (this->LoadingScreen == nullptr)
        {
            UE_LOG(
                LogGameManagementFramework,
                Error,
                TEXT("Call to NewObject<> failed when making the loading screen!"));
            return nullptr;
        }
    }

    return this->LoadingScreen;
}

bool UGameManagementSubsystem::Tick(float DeltaTime)
{
    auto DidFinishTask = false;
    auto WillHaveNextTask = false;
    if (this->ActiveTask != nullptr)
    {
        if (this->ActiveTask->Ready)
        {
            UE_LOG(
                LogGameManagementFramework,
                Verbose,
                TEXT("Current task has finished work %s"),
                *UKismetSystemLibrary::GetDisplayName(this->ActiveTask));

            if (this->TaskQueue.Num() > 0)
            {
                WillHaveNextTask = true;
            }

            this->ActiveTask = nullptr;
            DidFinishTask = true;
        }
        else
        {
            // Task is still running.
            return true;
        }
    }

    auto AllFutureTasksDontShowLoadingScreen = true;
    for (auto LSNeeded : this->LoadingScreenQueue)
    {
        if (LSNeeded)
        {
            AllFutureTasksDontShowLoadingScreen = false;
            break;
        }
    }
    if (this->TaskQueue.Num() == 0 || AllFutureTasksDontShowLoadingScreen)
    {
        // Hide loading screen.
        if (DidFinishTask)
        {
            auto LS = this->GetLoadingScreen();
            if (LS != nullptr)
            {
                UE_LOG(
                    LogGameManagementFramework,
                    Verbose,
                    TEXT("Hiding loading screen because all tasks are complete"));

                LS->HideLoadingScreen();
                this->bHasShownLoadingScreen = false;
            }
        }

        // No more tasks.
        if (this->TaskQueue.Num() == 0)
        {
            return true;
        }
    }

    if (this->ActiveTask == nullptr)
    {
        if (WillHaveNextTask && bHasShownLoadingScreen)
        {
            // Update existing loading screen.
            auto LS = this->GetLoadingScreen();
            if (LS != nullptr)
            {
                UE_LOG(
                    LogGameManagementFramework,
                    Verbose,
                    TEXT("Updating loading screen because of task %s"),
                    *UKismetSystemLibrary::GetDisplayName(this->TaskQueue[0]));

                LS->UpdateLoadingScreenStatus(this->TaskQueue[0]);
            }
        }
        else if (this->LoadingScreenQueue[0])
        {
            auto LS = this->GetLoadingScreen();
            if (LS != nullptr)
            {
                UE_LOG(
                    LogGameManagementFramework,
                    Verbose,
                    TEXT("Showing loading screen because of task %s"),
                    *UKismetSystemLibrary::GetDisplayName(this->TaskQueue[0]));

                LS->ShowLoadingScreen(this->TaskQueue[0]);
                this->bHasShownLoadingScreen = true;
            }
        }

        this->ActiveTask = this->TaskQueue[0];
        this->TaskQueue.RemoveAt(0);
        this->LoadingScreenQueue.RemoveAt(0);
        this->ActiveTask->Scheduled();

        UE_LOG(
            LogGameManagementFramework,
            Verbose,
            TEXT("Scheduled queued task %s"),
            *UKismetSystemLibrary::GetDisplayName(this->ActiveTask));
    }

    return true;
}

void UGameManagementSubsystem::EnqueueTask(UGameManagerTask *InTask, bool bShowLoadingScreen)
{
    this->TaskQueue.Add(InTask);
    this->LoadingScreenQueue.Add(bShowLoadingScreen);
}

void UGameManagementSubsystem::Initialize(FSubsystemCollectionBase &Collection)
{
    auto Config = GetMutableDefault<UGameManagementFrameworkConfig>();

    for (auto GM : Config->GameManagers)
    {
        auto ManagerClass = GM.Value.TryLoadClass<UGameManager>();
        if (ManagerClass == nullptr)
        {
            UE_LOG(LogGameManagementFramework, Error, TEXT("Could not find manager class for %s"), *GM.Key.ToString());
            continue;
        }

        auto Instance = NewObject<UGameManager>(this->GetOuter(), ManagerClass);
        this->GameManagerInstances.Add(GM.Key, Instance);
    }

    this->TickDelegateHandle =
        FUTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UGameManagementSubsystem::Tick));
}

void UGameManagementSubsystem::Deinitialize()
{
    FUTicker::GetCoreTicker().RemoveTicker(this->TickDelegateHandle);
}