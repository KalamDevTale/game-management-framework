// Copyright June Rhodes. All Rights Reserved.

#include "GameManager.h"
#include "Engine/Engine.h"
#include "GameManagementSubsystem.h"

FGameManagerTicker::FGameManagerTicker(TSoftObjectPtr<class UGameManager> Owner)
{
    this->GameManager = Owner;
    this->bHasCalledInit = false;
}

bool FGameManagerTicker::IsTickable() const
{
    if (!this->GameManager.IsValid())
    {
        // Game Manager no longer valid.
        return false;
    }

    if (this->GameManager->HasAnyFlags(RF_ClassDefaultObject))
    {
        // Do not tick on CDO.
        return false;
    }

    return true;
}

void FGameManagerTicker::Tick(float DeltaTime)
{
    if (this->GameManager.IsValid())
    {
        if (!this->bHasCalledInit)
        {
            this->GameManager->Init();
            this->bHasCalledInit = true;
        }

        this->GameManager->Tick(DeltaTime);
    }
}

UGameManager::UGameManager(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    this->Ticker = MakeShared<FGameManagerTicker>(this);
}

UWorld *UGameManager::GetWorld() const
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

UGameManagerTask *UGameManager::ScheduleTask(
    UObject *WorldContextObject,
    const FString &TaskId,
    TSubclassOf<UGameManagerTask> TaskType,
    EGameManagerTaskReadyState &ResultBranches)
{
    if (!TaskType->IsChildOf(UGameManagerTask::StaticClass()))
    {
        ResultBranches = EGameManagerTaskReadyState::NotReady;
        return nullptr;
    }

    if (Tasks.Contains(TaskId))
    {
        UGameManagerTask **TaskPtr = Tasks.Find(TaskId);
        if (TaskPtr == nullptr)
        {
            // TODO: Log.
            ResultBranches = EGameManagerTaskReadyState::NotReady;
            return nullptr;
        }

        UGameManagerTask *Task = *TaskPtr;

        if (Task->Ready)
        {
            ResultBranches = EGameManagerTaskReadyState::Ready;
        }
        else
        {
            ResultBranches = EGameManagerTaskReadyState::NotReady;
        }

        return Task;
    }
    else
    {
        // Construct it.
        UGameManagerTask *Task = NewObject<UGameManagerTask>(this, TaskType);

        // Add it to our scheduled tasks.
        Tasks.Add(TaskId, Task);

        // NOTE: We no longer call Scheduled here. Instead, BPNode_ScheduleGameManagerTask
        // calls Activate after assigning all the properties (Activate then returns whether
        // or not Scheduled immediately completed, and if it does, calls Ready instead of
        // Not Ready).

        // We've just created the task and are not ready. The wrapping node is responsible
        // for assigning the spawn variables, and it only does so on the CreatedNotReady
        // output branch.
        ResultBranches = EGameManagerTaskReadyState::CreatedNotReady;

        return Task;
    }
}

UGameManagerTask *UGameManager::EnqueueTask(
    UObject *WorldContextObject,
    TSubclassOf<UGameManagerTask> TaskType,
    bool bShowLoadingScreen)
{
    if (!TaskType->IsChildOf(UGameManagerTask::StaticClass()))
    {
        return nullptr;
    }

    UGameManagerTask *Task = NewObject<UGameManagerTask>(this, TaskType);
    auto Subsystem = WorldContextObject->GetWorld()->GetGameInstance()->GetSubsystem<UGameManagementSubsystem>();
    check(Subsystem);
    Subsystem->EnqueueTask(Task, bShowLoadingScreen);

    return Task;
}