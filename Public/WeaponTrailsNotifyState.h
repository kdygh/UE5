// 武器の軌跡演出を処理する。

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "WeaponTrailsNotifyState.generated.h"


// 武器の軌跡演出を処理する役割を持つ。
UCLASS()
class PJ26_API UWeaponTrailsNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:
    virtual void NotifyBegin(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        float TotalDuration,
        const FAnimNotifyEventReference& EventReference
    ) override;

    virtual void NotifyEnd(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        const FAnimNotifyEventReference& EventReference
    ) override;
};
