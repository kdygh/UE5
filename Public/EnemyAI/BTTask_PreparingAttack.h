// 敵の人工知能による行動を処理する。

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "EnemyOrigin.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "BTTask_PreparingAttack.generated.h"


class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;

// 敵の人工知能による行動を処理する役割を持つ。
UCLASS()
class PJ26_API UBTTask_PreparingAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	UBTTask_PreparingAttack();

	virtual EBTNodeResult::Type
		ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	// 実行中の行動を更新し、継続・成功・失敗の状態を判定する。
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	// 行動木の各実行単位が保持する記憶領域の大きさを返す。
	virtual uint16 GetInstanceMemorySize() const override;

private:
	UFUNCTION()
	void OnEQSQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEnvQuery> RepositionQuery;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
	float MinPrepareTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
	float MaxPrepareTime = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
	float AtkRange = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
	float RepositionInterval = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowPrivateAccess = true))
	float SearchRadius = 200.0f;

	UPROPERTY()
	TObjectPtr<AAIController> CachedController;
};
