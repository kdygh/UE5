// アニメーションとゲーム処理を連携する。

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyAnimInstancePj26.generated.h"


// アニメーションとゲーム処理を連携する役割を持つ。
UCLASS()
class PJ26_API UMyAnimInstancePj26 : public UAnimInstance
{
	GENERATED_BODY()

public:
    // 対象を指定された値へ更新する。
    UFUNCTION(BlueprintCallable)
    void SetIgnoreRootMotion(bool bIgnore);

    // 所有キャラクターの速度と空中状態をアニメーション用の値へ反映する。
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float Speed = 0.f;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool IsEvading = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool IsInAir = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool IsMoving = false;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float Forward = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float Right = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atk")
    bool IsAttacking = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atk")
    bool IsAttackingStrong = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Atk")
    bool IsPreparingAtk = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damaged")
    bool IsDamaged = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Death")
    bool IsDead = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LockOn")
    bool IsLockingOn = false;
};
