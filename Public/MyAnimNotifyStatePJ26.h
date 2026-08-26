// アニメーションとゲーム処理を連携する。

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MyAnimNotifyStatePJ26.generated.h"


// アニメーションとゲーム処理を連携する役割を持つ。
UCLASS()
class PJ26_API UMyAnimNotifyStatePJ26 : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float TotalDuration
	) override;

	virtual void NotifyTick(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float FrameDeltaTime
	) override {
	};

	virtual void NotifyEnd(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation
	) override;
};
