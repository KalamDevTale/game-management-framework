// Copyright June Rhodes. All Rights Reserved.

#include "GameManagerTask.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"

bool UGameManagerTask::IsTickable() const
{
    if (HasAnyFlags(RF_ClassDefaultObject))
    {
        return false;
    }

    if (!this->EnableTick)
    {
        return false;
    }

    return true;
}

ETickableTickType UGameManagerTask::GetTickableTickType() const
{
    return this->EnableTick ? ETickableTickType::Conditional : ETickableTickType::Never;
}

UWorld *UGameManagerTask::GetWorld() const
{
    UGameInstance *GameInstance = Cast<UGameInstance>(GetOuter()->GetOuter());

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

void UGameManagerTask::Activate(EGameManagerTaskReadyState &ResultBranches)
{
    this->Scheduled();

    // It's possible for Scheduled to immediately ready the task, so we return
    // whether we're now ready. BPNode_ScheduleGameManagerTask uses this to route
    // between Not Ready and Ready after creation.
    if (this->Ready)
    {
        ResultBranches = EGameManagerTaskReadyState::Ready;
    }
    else
    {
        ResultBranches = EGameManagerTaskReadyState::NotReady;
    }
}

void UGameManagerTask::Completed()
{
    Ready = true;
}