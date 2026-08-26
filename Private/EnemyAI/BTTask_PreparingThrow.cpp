// 敵の人工知能による行動を処理する。

#include "EnemyAI/BTTask_PreparingThrow.h"
#include "EnemyAI/LongDistAtkEnemyAIController.h"
#include "enemy/LongDistAtkEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
UBTTask_PreparingThrow::UBTTask_PreparingThrow()
{
	NodeName = TEXT("PrepareThrow");
	bNotifyTick = true;
}

// 行動木の実行条件を確認し、担当する敵行動を開始して結果を返す。
EBTNodeResult::Type UBTTask_PreparingThrow::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FPrepareAtkTaskMemory* Memory = (FPrepareAtkTaskMemory*)NodeMemory;
	if (!Memory)
	{
		return EBTNodeResult::Failed;
	}

	auto controller = Cast<ALongDistAtkEnemyAIController>(OwnerComp.GetAIOwner());
	auto blackboard = OwnerComp.GetBlackboardComponent();

	auto target = GetWorld()->GetFirstPlayerController()->GetPawn();
	APawn* aiPawn = controller->GetPawn();

	const float PrepareTime = FMath::FRandRange(MinPrepareTime, MaxPrepareTime);
	const float CurrentTime = aiPawn->GetWorld()->GetTimeSeconds();

	Memory->EndTime = CurrentTime + PrepareTime;
	Memory->NextMoveTime = CurrentTime;

	ALongDistAtkEnemy* zako = nullptr;
	zako = Cast<ALongDistAtkEnemy>(controller->GetPawn());

	if (!zako)
	{
		return EBTNodeResult::Failed;
	}

	zako->SearchPlayerPos();

	return EBTNodeResult::InProgress;

}

// 実行中の行動を更新し、継続・成功・失敗の状態を判定する。
void UBTTask_PreparingThrow::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FPrepareAtkTaskMemory* Memory = (FPrepareAtkTaskMemory*)NodeMemory;
	if (!Memory)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("UBTTask_PreparingDash::NoMeMORY"));
		return;
	}

	auto controller = Cast<ALongDistAtkEnemyAIController>(OwnerComp.GetAIOwner());

	APawn* aiPawn = controller->GetPawn();

	const float currentTime = aiPawn->GetWorld()->GetTimeSeconds();

	ALongDistAtkEnemy* zako = nullptr;
	zako = Cast<ALongDistAtkEnemy>(controller->GetPawn());
	if (!zako)
	{
		return;
	}

	if (zako->GetIsStunned())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}


	if (currentTime >= Memory->EndTime)
	{
		controller->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

}

// 行動木の各実行単位が保持する記憶領域の大きさを返す。
uint16 UBTTask_PreparingThrow::GetInstanceMemorySize() const
{
	return sizeof(FPrepareAtkTaskMemory);
}
