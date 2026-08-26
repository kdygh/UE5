// 敵の人工知能による行動を処理する。

#include "BTTask_Stun.h"
#include "EnemyOrigin.h"
#include "enemy/NormalEnemy.h"
#include "enemy/DashEnemy.h"
#include "EnemyAI/DashEnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
UBTTask_Stun::UBTTask_Stun()
{
	NodeName = TEXT("Stun");
	bNotifyTick = true;
}

// 行動木の実行条件を確認し、担当する敵行動を開始して結果を返す。
EBTNodeResult::Type UBTTask_Stun::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* const controller = OwnerComp.GetAIOwner();

	ADashEnemyAIController* const dashController = Cast<ADashEnemyAIController>(controller);

	dashController->GetBlackboardComponent()->SetValueAsBool(TEXT("isAbleToAtk"), false);

	return EBTNodeResult::InProgress;
}

// 実行中の行動を更新し、継続・成功・失敗の状態を判定する。
void UBTTask_Stun::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* const cont = OwnerComp.GetAIOwner();
	ANormalEnemy* const normalEnemy = Cast<ANormalEnemy>(cont->GetPawn());
	ADashEnemy* const dashEnemy = Cast<ADashEnemy>(cont->GetPawn());
	if (normalEnemy)
	{
		if (HasMontageFinished(normalEnemy))
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}
	if (dashEnemy)
	{
		if (HasMontageFinished(dashEnemy))
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}
}

// 処理の完了を確認し、後続の状態または画面へ移行する。
bool UBTTask_Stun::HasMontageFinished(AEnemyOrigin* const _zako)
{
	return _zako->GetMesh()->GetAnimInstance()->Montage_GetIsStopped(_zako->GetAnimMontage());
}
