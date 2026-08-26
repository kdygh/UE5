// 武器の軌跡演出を処理する。

#include "WeaponTrailsNotifyState.h"
#include "../MyPlayer.h"

// アニメーション通知の開始時に、所有者へ開始処理を伝える。
void UWeaponTrailsNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(
        MeshComp,
        Animation,
        TotalDuration,
        EventReference
    );

    if (!IsValid(MeshComp))
    {
        return;
    }

    if (AMyPlayer* Player = Cast<AMyPlayer>(MeshComp->GetOwner()))
    {
        Player->StartWeaponTrail();
    }
}

// アニメーション通知の終了時に、所有者へ終了処理を伝える。
void UWeaponTrailsNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(
        MeshComp,
        Animation,
        EventReference
    );

    if (!IsValid(MeshComp))
    {
        return;
    }
}
