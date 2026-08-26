// 敵の人工知能による行動を処理する。

#include "EnemyAI/BTTask_Throw.h"
#include "EnemyOrigin.h"
#include "enemy/Boss.h"
#include "BehaviorTree/BlackboardComponent.h"

// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
UBTTask_Throw::UBTTask_Throw()
{
	NodeName = TEXT("Throw");
	bNotifyTick = true;
}

// 行動木の実行条件を確認し、担当する敵行動を開始して結果を返す。
EBTNodeResult::Type UBTTask_Throw::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    return EBTNodeResult::Type();
}

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void UBTTask_Throw::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
}

// 処理の完了を確認し、後続の状態または画面へ移行する。
bool UBTTask_Throw::HasMontageFinished(AEnemyOrigin* const _zako)
{
	return _zako->GetMesh()->GetAnimInstance()->Montage_GetIsStopped(_zako->GetAnimMontage());
}
