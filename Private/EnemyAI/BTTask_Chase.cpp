// 敵の人工知能による行動を処理する。

#include "EnemyAI/BTTask_Chase.h"
#include "EnemyAI/WeakEnemyAIController.h"
#include "enemy/WeakEnemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BlackboardComponent.h"

// 行動木の実行条件を確認し、担当する敵行動を開始して結果を返す。
EBTNodeResult::Type UBTTask_Chase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto controller = Cast<AWeakEnemyAIController>(OwnerComp.GetAIOwner());
	auto blackboard = OwnerComp.GetBlackboardComponent();

	auto target = GetWorld()->GetFirstPlayerController()->GetPawn();

	bool isInRange = false;

	if (!target)
	{
		return EBTNodeResult::Failed;
	}
	else
	{
		AWeakEnemy* zako = nullptr;

		if (!controller)
		{
			return EBTNodeResult::Failed;
		}
		else
		{
			zako = Cast<AWeakEnemy>(controller->GetPawn());
		}

		float dist = 0.f;

		if (zako)
		{
			AtkRange = zako->GetAtkRange();
			dist = zako->GetDistanceTo(target);
		}

		if(zako->GetIsStunned() == true || zako->GetIsAttacking() == true)
		{
			return EBTNodeResult::Failed;
		}

		isInRange = dist <= AtkRange;
		controller->GetBlackboardComponent()->SetValueAsBool("isPlayerinAtkRange", isInRange);
	}

	controller->MoveToActor(target, 40.f, true, true, false, 0, true);
	return EBTNodeResult::Succeeded;
}

// 処理の完了を確認し、後続の状態または画面へ移行する。
bool UBTTask_Chase::HasMontageFinished(AWeakEnemy* const _zako)
{
	return _zako->GetMesh()->GetAnimInstance()->Montage_GetIsStopped(_zako->GetAnimMontage());
}
