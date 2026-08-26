// 敵の人工知能による行動を処理する。

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_Stun.generated.h"


class AEnemyOrigin;

// 敵の人工知能による行動を処理する役割を持つ。
UCLASS()
class PJ26_API UBTTask_Stun : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	UBTTask_Stun();

	virtual EBTNodeResult::Type
		ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 実行中の行動を更新し、継続・成功・失敗の状態を判定する。
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// 処理の完了を確認し、後続の状態または画面へ移行する。
	bool HasMontageFinished(AEnemyOrigin* const _zako);
};
