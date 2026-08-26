// 敵の人工知能による行動を処理する。

#include "EnemyAI/BTTask_PreparingAttack.h"
#include "EnemyAI/NormalEnemyAIController.h"
#include "enemy/NormalEnemy.h"
#include "EnemyOrigin.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "NavigationSystem.h"
#include "Kismet/KismetMathLibrary.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
UBTTask_PreparingAttack::UBTTask_PreparingAttack()
{
	NodeName = TEXT("PrepareAtk");
	bNotifyTick = true;
}

// 行動木の実行条件を確認し、担当する敵行動を開始して結果を返す。
EBTNodeResult::Type UBTTask_PreparingAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FPrepareAtkTaskMemory* Memory = (FPrepareAtkTaskMemory*)NodeMemory;
	if (!Memory)
	{
		return EBTNodeResult::Failed;
	}

	// 現在は使用していない処理。
	//auto controller = Cast<ANormalEnemyAIController>(OwnerComp.GetAIOwner());
	AAIController* const controller = OwnerComp.GetAIOwner();
	auto blackboard = OwnerComp.GetBlackboardComponent();

	auto target = GetWorld()->GetFirstPlayerController()->GetPawn();
	APawn* aiPawn = controller->GetPawn();

	const float PrepareTime = FMath::FRandRange(MinPrepareTime, MaxPrepareTime);
	const float CurrentTime = aiPawn->GetWorld()->GetTimeSeconds();

	CachedController = controller;

	Memory->EndTime = CurrentTime + PrepareTime;
	Memory->NextMoveTime = CurrentTime;

	return EBTNodeResult::InProgress;
}

// 実行中の行動を更新し、継続・成功・失敗の状態を判定する。
void UBTTask_PreparingAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FPrepareAtkTaskMemory* Memory = (FPrepareAtkTaskMemory*)NodeMemory;
	if (!Memory)
	{
		return;
	}

	// 現在は使用していない処理。
	//auto controller = Cast<ANormalEnemyAIController>(OwnerComp.GetAIOwner());
	AAIController* const controller = OwnerComp.GetAIOwner();
	auto blackboard = OwnerComp.GetBlackboardComponent();

	auto target = GetWorld()->GetFirstPlayerController()->GetPawn();
	APawn* aiPawn = controller->GetPawn();

	const float currentTime = aiPawn->GetWorld()->GetTimeSeconds();

	AEnemyOrigin* zako = nullptr;
	zako = Cast<AEnemyOrigin>(controller->GetPawn());
	if (!zako)
	{
		return;
	}

	if (zako->GetIsStunned())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}


	FVector MyLocation = aiPawn->GetActorLocation();
	FVector TargetLocation = target->GetActorLocation();

	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(MyLocation, TargetLocation);
	FRotator NewRot(0.f, LookAtRot.Yaw, 0.f);
	aiPawn->SetActorRotation(NewRot);


	if (currentTime >= Memory->NextMoveTime)
	{
		if (RepositionQuery)
		{
			UEnvQueryInstanceBlueprintWrapper* QueryInstance =
				UEnvQueryManager::RunEQSQuery(
					aiPawn,
					RepositionQuery,
					aiPawn,
					EEnvQueryRunMode::SingleResult,
					nullptr
				);

			if (QueryInstance)
			{
				QueryInstance->GetOnQueryFinishedEvent().AddDynamic(
					this,
					&UBTTask_PreparingAttack::OnEQSQueryFinished
				);
			}
		}

		Memory->NextMoveTime = currentTime + RepositionInterval;
	}

	bool isInRange = false;
	float dist = 0.f;

	AtkRange = zako->GetAtkRange();
	dist = zako->GetDistanceTo(target);

	isInRange = dist <= AtkRange;
	controller->GetBlackboardComponent()->SetValueAsBool("isPlayerinAtkRange", isInRange);


	if (currentTime >= Memory->EndTime)
	{
		controller->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
}

// 行動木の各実行単位が保持する記憶領域の大きさを返す。
uint16 UBTTask_PreparingAttack::GetInstanceMemorySize() const
{
	return sizeof(FPrepareAtkTaskMemory);
}

// 処理の完了を確認し、後続の状態または画面へ移行する。
void UBTTask_PreparingAttack::OnEQSQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (!QueryInstance || QueryStatus != EEnvQueryStatus::Success)
	{
		return;
	}

	if (!CachedController)
	{
		return;
	}

	TArray<FVector> Results = QueryInstance->GetResultsAsLocations();
	if (Results.Num() <= 0)
	{
		return;
	}

	int32 RandomIndex = FMath::RandRange(0, Results.Num() - 1);

	const FVector MoveLocation = Results[RandomIndex];

	CachedController->MoveToLocation(MoveLocation, 100.f, false);
}
