// ゲーム内オブジェクトの動作を処理する。

#include "Objects/DeathCameraActor.h"

// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
ADeathCameraActor::ADeathCameraActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void ADeathCameraActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    USkeletalMeshComponent* MeshComp =
        TargetMesh.Get();

    if (!IsValid(MeshComp))
    {
        return;
    }

    const FVector TargetLocation =
        MeshComp->GetSocketLocation(TEXT("pelvis"));

    const FVector DesiredLocation =
        TargetLocation + CameraOffset;

    const FVector NewLocation =
        FMath::VInterpTo(
            GetActorLocation(),
            DesiredLocation,
            DeltaTime,
            2.5f
        );

    SetActorLocation(NewLocation);

    const FRotator TargetRotation =
        (TargetLocation - NewLocation).Rotation();

    const FRotator NewRotation =
        FMath::RInterpTo(
            GetActorRotation(),
            TargetRotation,
            DeltaTime,
            4.0f
        );

    SetActorRotation(NewRotation);
}

// 対象を指定された値へ更新する。
void ADeathCameraActor::SetTarget(USkeletalMeshComponent* InTarget)
{
	TargetMesh = InTarget;
}
