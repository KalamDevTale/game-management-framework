// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "GameManager.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SubclassOf.h"

#include "GameManagementFrameworkBlueprintLibrary.generated.h"

UCLASS()
class GAMEMANAGEMENTFRAMEWORK_API UGameManagementFrameworkBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(
        BlueprintPure,
        Category = "Game Management Framework",
        meta = (WorldContext = "ContextObject", BlueprintInternalUseOnly = "true"))
    static UGameManager *GetGameManager(UObject *ContextObject, FName ManagerName);
};