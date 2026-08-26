// 敵の人工知能による行動を処理する。

#include "EnemyAI/MyBTTask_BlackboardBase.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "EnemyAI/NormalEnemyAIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BlackboardComponent.h"

// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
UMyBTTask_BlackboardBase::UMyBTTask_BlackboardBase()
{
	NodeName = TEXT("Find Random Location");


	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UMyBTTask_BlackboardBase, BlackboardKey));
}

// 行動木の実行条件を確認し、担当する敵行動を開始して結果を返す。
EBTNodeResult::Type UMyBTTask_BlackboardBase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FNavLocation RandomLocation{};

	AAIController* AIController{ OwnerComp.GetAIOwner() };
	const APawn* AIPawn{ AIController->GetPawn() };

	const FVector Origin{ AIPawn->GetActorLocation() };

	const UNavigationSystemV1* NavSys{ FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()) };

	auto controller = Cast<ANormalEnemyAIController>(OwnerComp.GetAIOwner());
	auto blackboard = OwnerComp.GetBlackboardComponent();


	if (IsValid(NavSys) &&
		NavSys->GetRandomPointInNavigableRadius(Origin, SearchRadius, RandomLocation))
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy Moving"));
		AIController->GetBlackboardComponent()->SetValueAsVector(BlackboardKey.SelectedKeyName, FVector(RandomLocation.Location.X, RandomLocation.Location.Y, Origin.Z));
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	return EBTNodeResult::Failed;
}

// 対象を取得し、呼び出し元へ返す。
FString UMyBTTask_BlackboardBase::GetStaticDescription() const
{
	return FString::Printf(TEXT("Vector : %s"),*BlackboardKey.SelectedKeyName.ToString());
}
