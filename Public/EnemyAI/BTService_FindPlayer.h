// 敵の人工知能による行動を処理する。

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_FindPlayer.generated.h"


// 敵の人工知能による行動を処理する役割を持つ。
UCLASS()
class PJ26_API UBTService_FindPlayer : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	UBTService_FindPlayer();

	// 一定間隔でプレイヤーの位置と検知状態を確認し、ブラックボードの値を更新する。
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// 対象に関するイベントを受け取り、関連状態を更新する。
	void OnSetTarget();
};
