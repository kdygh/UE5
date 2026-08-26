// 敵の人工知能による行動を処理する。

#include "EnemyAI/BTService_FindPlayer.h"
#include "../Public/EnemyAI/NormalEnemyAIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyOrigin.h"

// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
UBTService_FindPlayer::UBTService_FindPlayer()
{
	bCreateNodeInstance = true;
}

// 一定間隔でプレイヤーの位置と検知状態を確認し、ブラックボードの値を更新する。
void UBTService_FindPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// 現在は使用していない処理。
	//auto controller = Cast<ANormalEnemyAIController>(OwnerComp.GetAIOwner());
	AAIController* const controller = OwnerComp.GetAIOwner();

	auto playerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	if (controller)
	{
		OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Object>("Target", playerPawn);
	}
}

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void UBTService_FindPlayer::OnSetTarget()
{
}
