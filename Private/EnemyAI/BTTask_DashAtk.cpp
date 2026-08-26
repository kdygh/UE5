// 敵の人工知能による行動を処理する。

#include "EnemyAI/BTTask_DashAtk.h"
#include "EnemyOrigin.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "enemy/Boss.h"

// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
UBTTask_DashAtk::UBTTask_DashAtk()
{
	NodeName = TEXT("DashAtk");
	bNotifyTick = true;
}

// 行動木の実行条件を確認し、担当する敵行動を開始して結果を返す。
EBTNodeResult::Type UBTTask_DashAtk::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* const cont = OwnerComp.GetAIOwner();

	if (ABoss* const zako = Cast<ABoss>(cont->GetPawn()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("UBTTask_Attacking::ExecuteTask::Boss"));
		zako->DashAttack();
	}
	return EBTNodeResult::Type();
}

// 実行中の行動を更新し、継続・成功・失敗の状態を判定する。
void UBTTask_DashAtk::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* const cont = OwnerComp.GetAIOwner();

	ABoss* const boss = Cast<ABoss>(cont->GetPawn());

	if (HasMontageFinished(boss))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
}

// 処理の完了を確認し、後続の状態または画面へ移行する。
bool UBTTask_DashAtk::HasMontageFinished(AEnemyOrigin* const _zako)
{
	return _zako->GetMesh()->GetAnimInstance()->Montage_GetIsStopped(_zako->GetAnimMontage());
}
