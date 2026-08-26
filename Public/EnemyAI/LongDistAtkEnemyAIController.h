// 敵の人工知能による行動を処理する。

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BlackBoard/BlackboardKey.h"
#include <Perception/AIPerceptionTypes.h>
#include "LongDistAtkEnemyAIController.generated.h"


class UBehaviorTreeComponent;
class UBEhaviorTree;
class UAISenseConfig_Sight;
class AEnemyOrigin;

// 敵の人工知能による行動を処理する役割を持つ。
UCLASS()
class PJ26_API ALongDistAtkEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:

	FBlackboard::FKey targetKeyID;
	FBlackboard::FKey selfID;
	FBlackboard::FKey targetLocationID;
	FBlackboard::FKey didNoticedPlayerID;
	FBlackboard::FKey isPlayerinAtkRangeID;
	FBlackboard::FKey patrolPathVector;
	FBlackboard::FKey patrolPathIndex;

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	ALongDistAtkEnemyAIController();

	// 知覚した対象の情報を確認し、追跡状態をブラックボードへ反映する。
	UFUNCTION()
	void on_target_detected(AActor* actor, FAIStimulus const stimulus);

	// 視界を指定された値へ更新する。
	void SetSightRadius(bool isDetected);
protected:
	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;
	// 操作対象を取得した直後に、行動木・ブラックボード・知覚機能を初期化する。
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = true))
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponentAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = true))
	UBlackboardComponent* BlackboardComponentAsset;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Anim", meta = (AllowPrivateAccess = "true"))
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Anim", meta = (AllowPrivateAccess = "true"))
	AEnemyOrigin* EnemyOrigin;

private:
	// 現在は使用していない処理。
	// 敵の視認距離・視野角・検知対象を設定して知覚機能を準備する。
	//void setup_perception_system(ACharacterOrigin* _zako);
	// 敵の視認距離・視野角・検知対象を設定して知覚機能を準備する。
	void setup_perception_system();
};
