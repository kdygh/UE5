// ゲーム内オブジェクトの動作を処理する。

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UEnemyAttackQueueComponent.generated.h"

class AEnemyOrigin;

// ゲーム内オブジェクトの動作を処理する役割を持つ。
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PJ26_API UUEnemyAttackQueueComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    // 攻撃の利用を要求し、取得可能か判定する。
    bool RequestAttack(AEnemyOrigin* Enemy);
    // 攻撃の利用状態を解除し、管理先へ返却する。
    void ReleaseAttack(AEnemyOrigin* Enemy);
    // 敵を管理対象から削除し、保持状態を整理する。
    void RemoveEnemy(AEnemyOrigin* Enemy);

    // 攻撃の条件を満たしているか判定する。
    bool HasAttackToken(const AEnemyOrigin* Enemy) const;

protected:
    UPROPERTY(EditAnywhere, Category = "Attack Queue")
    int32 MaxConcurrentAttackers = 1;

    UPROPERTY()
    TArray<TObjectPtr<AEnemyOrigin>> WaitingEnemies;

    UPROPERTY()
    TSet<TObjectPtr<AEnemyOrigin>> ActiveAttackers;

    // 攻撃待ち行列の先頭を確認し、次の敵へ攻撃権を付与する。
    void TryGrantNext();
};
