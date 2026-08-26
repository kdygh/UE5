// アニメーションとゲーム処理を連携する。

#include "MyAnimInstancePj26.h"
#include "MyPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

// 対象を指定された値へ更新する。
void UMyAnimInstancePj26::SetIgnoreRootMotion(bool bIgnore)
{
    if (bIgnore)
    {
        RootMotionMode = ERootMotionMode::IgnoreRootMotion;
    }
    else
    {
        RootMotionMode = ERootMotionMode::RootMotionFromMontagesOnly;

    }
}

// 所有キャラクターの速度と空中状態をアニメーション用の値へ反映する。
void UMyAnimInstancePj26::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    APawn* Pawn = TryGetPawnOwner();
    if (!Pawn)
    {
        return;
    }

    FVector Velocity = Pawn->GetVelocity();
    Velocity.Z = 0.0f;
    Speed = Velocity.Size();

    FVector LocalVelocity = Pawn->GetActorTransform().InverseTransformVectorNoScale(Velocity);

    ACharacter* Character = Cast<ACharacter>(Pawn);
    if (Character && Character->GetCharacterMovement()->MaxWalkSpeed > KINDA_SMALL_NUMBER)
    {
        float MaxSpeed = Character->GetCharacterMovement()->MaxWalkSpeed;
        Forward = LocalVelocity.X / MaxSpeed;
        Right = LocalVelocity.Y / MaxSpeed;
    }

    if (AMyPlayer* MyPlayer = Cast<AMyPlayer>(Pawn))
    {
        IsLockingOn = MyPlayer->GetIsLockingOn();
    }
}
