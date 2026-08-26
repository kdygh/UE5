// 敵の人工知能による行動を処理する。

#include "EnemyAI/BTTask_MoveToPlayer.h"
#include "EnemyAI/NormalEnemyAIController.h"
#include "EnemyOrigin.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type
// 行動木の実行条件を確認し、担当する敵行動を開始して結果を返す。
UBTTask_MoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* const controller = OwnerComp.GetAIOwner();
	// 現在は使用していない処理。
	//auto controller = Cast<ANormalEnemyAIController>(OwnerComp.GetAIOwner());
	auto blackboard = OwnerComp.GetBlackboardComponent();

	auto target = GetWorld()->GetFirstPlayerController()->GetPawn();

	bool isInRange = false;

	if(!target)
	{
		return EBTNodeResult::Failed;
	}
	else
	{
		AEnemyOrigin* zako = nullptr;

		if (!controller)
		{
			return EBTNodeResult::Failed;
		}
		else
		{
			zako = Cast<AEnemyOrigin>(controller->GetPawn());
		}

		float dist = 0.f;

		if (zako)
		{
			AtkRange = zako->GetAtkRange();
			dist = zako->GetDistanceTo(target);
		}

		isInRange = dist <= AtkRange;
		controller->GetBlackboardComponent()->SetValueAsBool("isPlayerinAtkRange", isInRange);
		// 現在は使用していない処理。
		//controller->SetSightRadius(isInRange);
	}
	controller->MoveToActor(target, 40.f, true, true, false, 0, true);

	return EBTNodeResult::Succeeded;
}

// 処理の完了を確認し、後続の状態または画面へ移行する。
bool UBTTask_MoveToPlayer::HasMontageFinished(AEnemyOrigin* const _zako)
{
	return _zako->GetMesh()->GetAnimInstance()->Montage_GetIsStopped(_zako->GetAnimMontage());
}
