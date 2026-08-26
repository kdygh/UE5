// ゲーム内オブジェクトの動作を処理する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PooledActor.generated.h"

// ゲーム内オブジェクトの動作を処理する役割を持つ。
UCLASS()
class PJ26_API APooledActor : public AActor
{
	GENERATED_BODY()

public:
	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	APooledActor();

	// プールから取得したアクターの位置と状態を再設定し、再び有効にする。
	virtual void ActivateFromPool(const FTransform& SpawnTransform);
	// 表示・衝突・更新を無効にし、アクターをプールで待機する状態へ戻す。
	virtual void DeactivateToPool();

	// 方向を指定された値へ更新する。
	void SetMoveDirection(const FVector& NewDirection) { MoveDirection = NewDirection; }

	// 対象の条件を満たしているか判定する。
	bool IsInUse() const { return bInUse; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pool")
	bool bInUse = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightning", meta = (AllowPrivateAccess = "true"))
	FVector MoveDirection = FVector::ZeroVector;

protected:
	float currentTime = 0.f;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightning", meta = (AllowPrivateAccess = "true"))
	float Duration = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightning", meta = (AllowPrivateAccess = "true"))
	float MaxScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightning", meta = (AllowPrivateAccess = "true"))
	float MoveSpeed = 0.f;
};
