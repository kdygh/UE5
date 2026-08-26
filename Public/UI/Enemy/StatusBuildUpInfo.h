// 敵固有の行動と戦闘を処理する。

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StatusBuildUpInfo.generated.h"


// 敵固有の行動と戦闘を処理する役割を持つ。
USTRUCT(BlueprintType)
struct FStatusBuildUpInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag DebuffTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DebuffAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxDebuffAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> IconTexture = nullptr;
};
