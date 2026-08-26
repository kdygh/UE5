// 武器の軌跡演出を処理する。

#include "Components/WeaponTrailComponent.h"
#include "NiagaraComponent.h"
#include <NiagaraFunctionLibrary.h>


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
UWeaponTrailComponent::UWeaponTrailComponent()
{


	PrimaryComponentTick.bCanEverTick = false;


}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void UWeaponTrailComponent::BeginPlay()
{
	Super::BeginPlay();


}

// 現在武器軌跡システムを取得し、呼び出し元へ返す。
UNiagaraSystem* UWeaponTrailComponent::GetCurrentWeaponTrailSystem(EAttackAttribute CurrentAttribute) const
{
    switch (CurrentAttribute)
    {
    case EAttackAttribute::Fire:
        return FireWeaponTrail;

    case EAttackAttribute::Wind:
        return WindWeaponTrail;

    case EAttackAttribute::Lightning:
        return LightningWeaponTrail;

    case EAttackAttribute::None:
    default:
        return DefaultWeaponTrail;
    }
}

// 攻撃属性に応じた武器軌跡を開始する。
void UWeaponTrailComponent::StartWeaponTrail(USkeletalMeshComponent* Mesh, EAttackAttribute CurrentAttribute)
{
    UNiagaraSystem* TrailSystem = GetCurrentWeaponTrailSystem(CurrentAttribute);

    if (!IsValid(TrailSystem) || !IsValid(Mesh))
    {
        return;
    }

    StopWeaponTrail();

    ActiveWeaponTrail =
        UNiagaraFunctionLibrary::SpawnSystemAttached(
            TrailSystem,
            Mesh,
            TEXT("hand_r"),
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            FVector::OneVector,
            EAttachLocation::SnapToTarget,
            false,
            ENCPoolMethod::None,
            true,
            true
        );
}

// 武器軌跡を終了し、関連状態とタイマーを解除する。
void UWeaponTrailComponent::StopWeaponTrail()
{
    if (!IsValid(ActiveWeaponTrail))
    {
        return;
    }

    ActiveWeaponTrail->Deactivate();
    ActiveWeaponTrail = nullptr;
}
