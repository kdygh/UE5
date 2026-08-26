// 敵の人工知能による行動を処理する。

#include "EnemyAI/BossAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Components/CapsuleComponent.h"
#include <NavigationSystem.h>
#include "../MyPlayer.h"
#include "EnemyOrigin.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"

// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
ABossAIController::ABossAIController()
{
	BehaviorTreeComponentAsset = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponentAsset = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	bAttachToPawn = true;

	setup_perception_system();
}

// 知覚した対象の情報を確認し、追跡状態をブラックボードへ反映する。
void ABossAIController::on_target_detected(AActor* actor, FAIStimulus const stimulus)
{
	if (EnemyOrigin)
	{
		if (auto const ch = Cast<AMyPlayer>(actor))
		{
			BlackboardComponentAsset->SetValueAsBool(FName("didNoticePlayer"), stimulus.WasSuccessfullySensed());

			EnemyOrigin->OnFollowPlayer(stimulus.WasSuccessfullySensed());

			UE_LOG(LogTemp, Log, TEXT("Enemy has detected player"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("player is nullptr"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("EnemyOrigin is nullptr"));
	}
}

// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void ABossAIController::BeginPlay()
{
	Super::BeginPlay();
}

// 操作対象を取得した直後に、行動木・ブラックボード・知覚機能を初期化する。
void ABossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	EnemyOrigin = Cast<AEnemyOrigin>(InPawn);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[this, InPawn]()
		{
			if (!InPawn) return;

			UE_LOG(LogTemp, Warning, TEXT("Controller class: %s"), *GetClass()->GetName());

			if (!BehaviorTreeAsset)
			{
				UE_LOG(LogTemp, Warning, TEXT("Behavior Tree asset is missing!"));
				return;
			}

			targetKeyID = BlackboardComponentAsset->GetKeyID("Target");
			selfID = BlackboardComponentAsset->GetKeyID("SelfActor");
			targetLocationID = BlackboardComponentAsset->GetKeyID("TargetLocation");
			didNoticedPlayerID = BlackboardComponentAsset->GetKeyID("didNoticePlayer");
			isPlayerinAtkRangeID = BlackboardComponentAsset->GetKeyID("isPlayerinAtkRange");

			UBlackboardData* BBAsset = BehaviorTreeAsset->BlackboardAsset;
			if (!BBAsset)
			{
				UE_LOG(LogTemp, Warning, TEXT("Blackboard asset is missing in Behavior Tree!"));
				return;
			}

			UseBlackboard(BBAsset, BlackboardComponentAsset);

			RunBehaviorTree(BehaviorTreeAsset);

			UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
			if (!NavSys)
			{
				UE_LOG(LogTemp, Error, TEXT("NavSys = null"));
				return;
			}

			ANavigationData* NavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);

			const FVector Start = InPawn->GetNavAgentLocation();

			FNavLocation Projected;
			const bool bProjected = NavSys->ProjectPointToNavigation(
				Start,
				Projected,
				FVector(200.f, 200.f, 300.f),
				NavData
			);

			ACharacter* Character = Cast<ACharacter>(InPawn);
			if (!Character)
			{
				UE_LOG(LogTemp, Error, TEXT("Character cast failed"));
				return;
			}

			UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
			if (!MoveComp)
			{
				UE_LOG(LogTemp, Error, TEXT("CharacterMovement null"));
				return;
			}

			targetKeyID = BlackboardComponentAsset->GetKeyID("Target");
			selfID = BlackboardComponentAsset->GetKeyID("SelfActor");
			targetLocationID = BlackboardComponentAsset->GetKeyID("TargetLocation");
			didNoticedPlayerID = BlackboardComponentAsset->GetKeyID("didNoticePlayer");
			patrolPathVector = BlackboardComponentAsset->GetKeyID("patrolPathVector");
			patrolPathIndex = BlackboardComponentAsset->GetKeyID("patrolPathIndex");
		},
		0.2f,
		false
	);

}

// 敵の視認距離・視野角・検知対象を設定して知覚機能を準備する。
void ABossAIController::setup_perception_system()
{
	UE_LOG(LogTemp, Log, TEXT("setup_perception_system on"));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	UAIPerceptionComponent* aiPercepComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
	SetPerceptionComponent(*aiPercepComp);

	SightConfig->SightRadius = 800.f;
	SightConfig->LoseSightRadius = SightConfig->SightRadius + 200.f;
	SightConfig->PeripheralVisionAngleDegrees = 360.f;
	SightConfig->SetMaxAge(1.f);
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 850.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	auto perceptionComp = GetPerceptionComponent();

	if (perceptionComp)
	{
		perceptionComp->SetDominantSense(*SightConfig->GetSenseImplementation());
		perceptionComp->OnTargetPerceptionUpdated.__Internal_AddDynamic(this, &ABossAIController::on_target_detected, FName("on_target_detected"));
		perceptionComp->ConfigureSense(*SightConfig);
	}
}
