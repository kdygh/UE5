// ロックオン対象の検索と追従を処理する。

#include "Components/CameraLockOnComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "../../MyPlayer.h"
#include "../Public/EnemyOrigin.h"
#include "../Public/UI/LockonWidget.h"
#include <Kismet/KismetSystemLibrary.h>


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
UCameraLockOnComponent::UCameraLockOnComponent()
{


	PrimaryComponentTick.bCanEverTick = true;


}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void UCameraLockOnComponent::BeginPlay()
{
	Super::BeginPlay();

    if (PlayerLockonWidgetClass)
    {
        PlayerLockonWidgetInstance = CreateWidget<ULockonWidget>(GetWorld(), PlayerLockonWidgetClass);
        if (PlayerLockonWidgetInstance)
        {
            PlayerLockonWidgetInstance->AddToViewport();
            PlayerLockonWidgetInstance->HideLockOnMarker();
        }
    }
}


// 毎フレーム、構成要素が担当する追従処理と状態確認を行う。
void UCameraLockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


    UpdateLockOn(DeltaTime);
}

// 周囲の候補を画面正面との角度で比較し、最適な対象をロックオン対象に設定する。
void UCameraLockOnComponent::FindBestTarget()
{
    TArray<AActor*> Targets;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    TArray<AActor*> IgnoreActors;

    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel5));
    IgnoreActors.Add(OwnerPlayer);

    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        OwnerPlayer->GetActorLocation(),
        LockOnRadius,
        ObjectTypes,
        LockOnTargetClass,
        IgnoreActors,
        Targets
    );

    AActor* BestTarget = nullptr;
    float BestScore = MinDot;

    const FVector ViewLoc = GetLockOnTargetLocation();
    const FVector ViewForward = GetLockOnTargetForward();

    for (AActor* Target : Targets)
    {
        if (!IsTargetValid(Target))
        {
            continue;
        }

        const FVector ToTarget = (Target->GetActorLocation() - ViewLoc).GetSafeNormal();
        const float Dot = FVector::DotProduct(ViewForward, ToTarget);

        if (Dot > BestScore)
        {
            BestScore = Dot;
            BestTarget = Target;
        }
    }

    LockedTarget = BestTarget;
}

// 現在の対象を基準に左右の候補を比較し、指定方向の対象へ切り替える。
void UCameraLockOnComponent::FindSideTarget(bool bRight)
{
    if (!LockedTarget)
    {
        return;
    }

    TArray<AActor*> Targets;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    TArray<AActor*> IgnoreActors;

    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel5));
    IgnoreActors.Add(OwnerPlayer);

    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        OwnerPlayer->GetActorLocation(),
        LockOnRadius,
        ObjectTypes,
        LockOnTargetClass,
        IgnoreActors,
        Targets
    );

    const FVector ViewLoc = GetLockOnTargetLocation();
    const FVector ViewRight = Camera ? Camera->GetRightVector() : OwnerPlayer->GetActorRightVector();

    AActor* BestTarget = nullptr;
    float BestSideValue = bRight ? 0.1f : -0.1f;

    for (AActor* Target : Targets)
    {
        if (!IsTargetValid(Target) || Target == LockedTarget)
        {
            continue;
        }

        const FVector ToTarget = (Target->GetActorLocation() - ViewLoc).GetSafeNormal();
        const float SideDot = FVector::DotProduct(ViewRight, ToTarget);

        if (bRight)
        {
            if (SideDot > BestSideValue)
            {
                BestSideValue = SideDot;
                BestTarget = Target;
            }
        }
        else
        {
            if (SideDot < BestSideValue)
            {
                BestSideValue = SideDot;
                BestTarget = Target;
            }
        }
    }

    LockedTarget = BestTarget;
}

// 対象の存在・距離・生存状態・遮蔽物を確認し、追跡可能か判定する。
bool UCameraLockOnComponent::IsTargetValid(AActor* Target) const
{
    if (!Target || Target == OwnerPlayer)
    {
        return false;
    }

    if (FVector::DistSquared(OwnerPlayer->GetActorLocation(), Target->GetActorLocation()) > FMath::Square(LoseDistance))
    {
        return false;
    }

    AEnemyOrigin* Enemy = Cast<AEnemyOrigin>(Target);

    if (Enemy->GetIsAlive() == false)
    {
        return false;
    }

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(OwnerPlayer);

    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        GetLockOnTargetLocation(),
        Target->GetActorLocation(),
        VisibilityChannel,
        Params
    );

    return !bHit || Hit.GetActor() == Target;

}

// 対象の有効性を確認し、照準表示とキャラクターの追従状態を更新する。
void UCameraLockOnComponent::UpdateLockOn(float DeltaTime)
{
    if (OwnerPlayer->GetIsLockingOn() == false && PlayerLockonWidgetInstance)
    {
        PlayerLockonWidgetInstance->HideLockOnMarker();
    }
    else if (OwnerPlayer->GetIsLockingOn() == true && PlayerLockonWidgetInstance)
    {
        PlayerLockonWidgetInstance->UpdateLockOnMarker(LockedTarget);
    }

    if (!LockedTarget)
    {
        LockedTarget = nullptr;
        OwnerPlayer->ClearLockOn();
        return;
    }

    if (!IsTargetValid(LockedTarget))
    {
        LockedTarget = nullptr;
        OwnerPlayer->ClearLockOn();
        return;
    }

    OwnerPlayer->SetIsLockingOn(true);
    FaceLockOnTarget(DeltaTime);
}

// キャラクターとカメラを対象方向へ滑らかに補間回転させる。
void UCameraLockOnComponent::FaceLockOnTarget(float DeltaTime)
{
    if (!LockedTarget)
    {
        return;
    }

    FVector ToTarget = LockedTarget->GetActorLocation() - OwnerPlayer->GetActorLocation();
    ToTarget.Z = 0.f;

    if (ToTarget.IsNearlyZero())
    {
        return;
    }

    const FRotator TargetYawRot = ToTarget.Rotation();


    const FRotator NewActorRot = FMath::RInterpTo(
        OwnerPlayer->GetActorRotation(),
        TargetYawRot,
        DeltaTime,
        CharacterRotateSpeed
    );
    OwnerPlayer->SetActorRotation(NewActorRot);


    if (APlayerController* PC = Cast<APlayerController>(OwnerPlayer->GetController()))
    {
        FRotator CurrentControlRot = PC->GetControlRotation();
        FRotator DesiredControlRot = CurrentControlRot;
        DesiredControlRot.Yaw = TargetYawRot.Yaw;

        const FRotator NewControlRot = FMath::RInterpTo(
            CurrentControlRot,
            DesiredControlRot,
            DeltaTime,
            CameraRotateSpeed
        );

        PC->SetControlRotation(NewControlRot);
    }
}

// カメラが有効ならカメラ位置を、無効ならプレイヤー位置を返す。
FVector UCameraLockOnComponent::GetLockOnTargetLocation() const
{
    return Camera ? Camera->GetComponentLocation() : OwnerPlayer->GetActorLocation();

}

// カメラが有効ならカメラ前方を、無効ならプレイヤー前方を返す。
FVector UCameraLockOnComponent::GetLockOnTargetForward() const
{
    return Camera ? Camera->GetForwardVector() : OwnerPlayer->GetActorForwardVector();
}

// ロックオンで使用するカメラを設定する。
void UCameraLockOnComponent::SetCamera(UCameraComponent* InCamera)
{
    this->Camera = InCamera;
}

// ロックオンを使用するプレイヤーを設定する。
void UCameraLockOnComponent::SetOwnerPlayer(AMyPlayer* InPlayer)
{
    OwnerPlayer = InPlayer;
}

// 対象参照を解除し、回転設定と照準表示を通常状態へ戻す。
void UCameraLockOnComponent::ClearLockOn()
{
    LockedTarget = nullptr;
    OwnerPlayer->GetCharacterMovement()->bOrientRotationToMovement = true;
}
