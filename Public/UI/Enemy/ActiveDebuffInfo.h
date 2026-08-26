// 敵固有の行動と戦闘を処理する。

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ActiveDebuffInfo.generated.h"


// 敵固有の行動と戦闘を処理する役割を持つ。
USTRUCT(BlueprintType)
struct FActiveDebuffInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag DebuffTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RemainingTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TotalDuration = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 State = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> IconTexture = nullptr;
};
