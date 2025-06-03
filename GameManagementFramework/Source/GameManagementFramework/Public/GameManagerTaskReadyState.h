// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"

#include "GameManagerTaskReadyState.generated.h"

UENUM(BlueprintType)
enum class EGameManagerTaskReadyState : uint8
{
    Ready,
    NotReady,
    CreatedNotReady
};