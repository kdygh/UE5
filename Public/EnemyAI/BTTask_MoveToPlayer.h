// 敵の人工知能による行動を処理する。

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_MoveToPlayer.generated.h"


class AEnemyOrigin;
// 敵の人工知能による行動を処理する役割を持つ。
UCLASS()
class PJ26_API UBTTask_MoveToPlayer : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type
		ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 処理の完了を確認し、後続の状態または画面へ移行する。
	bool HasMontageFinished(AEnemyOrigin* const _zako);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
	float AtkRange = 200.f;
};
