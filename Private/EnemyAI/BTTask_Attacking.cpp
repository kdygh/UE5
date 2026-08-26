// 敵の人工知能による行動を処理する。

#include "EnemyAI/BTTask_Attacking.h"
#include "EnemyOrigin.h"
#include "enemy/NormalEnemy.h"
#include "enemy/DashEnemy.h"
#include "enemy/LongDistAtkEnemy.h"
#include "enemy/WeakEnemy.h"
#include "enemy/Boss.h"
#include "EnemyAI/DashEnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
UBTTask_Attacking::UBTTask_Attacking()
{
	NodeName = TEXT("Atk");
	bNotifyTick = true;
}

// 行動木の実行条件を確認し、担当する敵行動を開始して結果を返す。
EBTNodeResult::Type UBTTask_Attacking::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* const cont = OwnerComp.GetAIOwner();

	AEnemyOrigin* const zako = Cast<AEnemyOrigin>(cont->GetPawn());

	if(zako)
	{
		zako->MeleeAttack();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("UBTTask_Attacking::ExecuteTask::zako is nullptr"));
	}

	return EBTNodeResult::InProgress;
}

// 実行中の行動を更新し、継続・成功・失敗の状態を判定する。
void UBTTask_Attacking::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* const cont = OwnerComp.GetAIOwner();
	ANormalEnemy* const normalEnemy = Cast<ANormalEnemy>(cont->GetPawn());
	ADashEnemy* const dashEnemy = Cast<ADashEnemy>(cont->GetPawn());
	AWeakEnemy* const weakEnemy = Cast<AWeakEnemy>(cont->GetPawn());
	ABoss* const boss = Cast<ABoss>(cont->GetPawn());
	if (normalEnemy)
	{
		if (HasMontageFinished(normalEnemy))
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}
	if (weakEnemy)
	{
		float dist = weakEnemy->GetDistanceTo(GetWorld()->GetFirstPlayerController()->GetPawn());
		bool isInRange = dist <= weakEnemy->GetAtkRange();
		cont->GetBlackboardComponent()->SetValueAsBool("isPlayerinAtkRange", isInRange);
		if (!isInRange)
		 {
			 FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			 return;
		}

		if (HasMontageFinished(weakEnemy))
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}
	if(boss)
	{
		if (HasMontageFinished(boss))
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}
	if(dashEnemy)
	{
		if (dashEnemy->GetIsHitByObject() == true)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("UBTTask_Attacking::HitByObject1"));

			if (UBlackboardComponent* BB = cont->GetBlackboardComponent())
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("UBTTask_Attacking::HitByObject2"));
				BB->SetValueAsBool(TEXT("isAbleToAtk"), false);

				const bool bValue = BB->GetValueAsBool(TEXT("isAbleToAtk"));
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, bValue ? TEXT("true") : TEXT("false"));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("No Blackboard Component"));
			}

			cont->StopMovement();
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}
}

// 処理の完了を確認し、後続の状態または画面へ移行する。
bool UBTTask_Attacking::HasMontageFinished(AEnemyOrigin* const _zako)
{
	return _zako->GetMesh()->GetAnimInstance()->Montage_GetIsStopped(_zako->GetAnimMontage());
}
