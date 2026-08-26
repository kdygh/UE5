// 部屋の生成と戦闘進行を管理する。

#include "Room.h"
#include "Public/Subsystem/RoomSubsystem.h"
#include "Public/Objects/EnemySpawner.h"
#include "Public/Objects/ItemSpawner.h"
#include "Public/Item/Item.h"
#include "Subsystem/GameAudioSubsystem.h"
#include "Components/BoxComponent.h"
#include "EnemyOrigin.h"
#include "Public/Subsystem/RoomSubsystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "enemy/NormalEnemy.h"
#include "enemy/DashEnemy.h"
#include "enemy/LongDistAtkEnemy.h"
#include "enemy/WeakEnemy.h"
#include "MyPlayer.h"
#include "EnemyAI/DashEnemyAIController.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
ARoom::ARoom()
{

	PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

	TopMiddleWall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TopMiddleWall"));
	TopMiddleWall->SetupAttachment(Root);

	BottomMiddleWall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BottomMiddleWall"));
    BottomMiddleWall->SetupAttachment(Root);

	LeftMiddleWall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftMiddleWall"));
    LeftMiddleWall->SetupAttachment(Root);

	RightMiddleWall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightMiddleWall"));
    RightMiddleWall->SetupAttachment(Root);

	RoomEnteranceTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("RoomEnteranceTrigger"));
	RoomEnteranceTrigger->SetupAttachment(Root);
}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void ARoom::BeginPlay()
{
	Super::BeginPlay();

	if (RoomEnteranceTrigger)
	{
		RoomEnteranceTrigger->OnComponentBeginOverlap.AddDynamic(
			this,
			&ARoom::HandleRoomTriggerBeginOverlap
		);

		UE_LOG(LogTemp, Warning, TEXT("Overlap bound successfully"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RoomEnteranceTrigger is null"));
	}

	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);

	for (AActor* Actor : AttachedActors)
	{
		if (IsValid(Actor))
		{
			AEnemySpawner* Spawner = Cast<AEnemySpawner>(Actor);
			AItemSpawner* ItemSpawner = Cast<AItemSpawner>(Actor);
			if (IsValid(Spawner))
			{
				EnemySpawners.Add(Spawner);
			}
			else if (IsValid(ItemSpawner))
			{
				ItemSpawners.Add(ItemSpawner);
			}
		}
	}
}

// 構成要素の生成完了後に、相互参照とイベントを接続する。
void ARoom::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	// 現在は使用していない処理。
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Player Entered Room"));
	//RoomEnteranceTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARoom::HandleRoomTriggerBeginOverlap);
}

// 終了時に登録済みイベントやタイマーを解除し、保持参照を整理する。
void ARoom::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Room EndPlay Reason=%d"),
		static_cast<int32>(EndPlayReason)
	);

	EnemySpawners.Empty();
	ItemSpawners.Empty();
	SpawnedItemList.Empty();
	SpawnedEnemyList.Empty();
	EnemyInAtkRangeList.Empty();

	Super::EndPlay(EndPlayReason);
}

// 重複実行を防ぎ、部屋に登録されたすべての敵から状態異常を解除する。
void ARoom::CleanRoom()
{
	if (IsRoomCleaned == true)
	{
		return;
	}

	IsRoomCleaned = true;

	for (auto enemy : EnemyInAtkRangeList)
	{
		if (IsValid(enemy))
		{
			enemy->RemoveAllDebuffs();
		}
	}

	for (auto enemy : SpawnedEnemyList)
	{
		if (IsValid(enemy))
		{
			enemy->RemoveAllDebuffs();
		}
	}
}


// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void ARoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerInRoom && IsValid(PlayerInRoom) &&
		!PlayerInRoom->GetIsAlive())
	{
		CleanRoom();
		return;
	}

	GetEnemyGrantAttack();
	SetEnemyGrantAttack();
}

// プレイヤーを生成し、位置・所有者・初期状態を設定する。
void ARoom::SpawnPlayer()
{
	FVector PlayerSpawnLoc = GetActorLocation() + FVector(0.f, 0.f, 100.f);

	GetWorld()->SpawnActor<APawn>(
		PlayerClass,
		PlayerSpawnLoc,
		FRotator::ZeroRotator
	);
}

// 対象を指定された値へ更新する。
void ARoom::SetDisabledWallsEnable()
{
	if (TopMiddleWall->IsVisible() == false)
	{
		TopMiddleWall->SetVisibility(true);
		TopMiddleWall->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DisabledWalls.Add(TopMiddleWall);
	}
	if(BottomMiddleWall->IsVisible() == false)
	{
		BottomMiddleWall->SetVisibility(true);
		BottomMiddleWall->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DisabledWalls.Add(BottomMiddleWall);
	}
	if(LeftMiddleWall->IsVisible() == false)
	{
		LeftMiddleWall->SetVisibility(true);
		LeftMiddleWall->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DisabledWalls.Add(LeftMiddleWall);
	}
	if(RightMiddleWall->IsVisible() == false)
	{
		RightMiddleWall->SetVisibility(true);
		RightMiddleWall->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DisabledWalls.Add(RightMiddleWall);
	}
}

// 対象を指定された値へ更新する。
void ARoom::SetEnabledWallsDisable()
{
	for(UStaticMeshComponent* Wall : DisabledWalls)
	{
		if(Wall)
		{
			Wall->SetVisibility(false);
			Wall->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

// 部屋に関するイベントを受け取り、関連状態を更新する。
void ARoom::HandleRoomTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor))
	{
		if (AMyPlayer* Player = Cast<AMyPlayer>(OtherActor))
		{
			PlayerInRoom = Player;

		}
	}

	RoomEnteranceTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Player Entered Room"));
	if(RoomType == ERoomType::COMBAT_ROOM)
	{
		StartBattle();
	}
	else if(RoomType == ERoomType::ITEM_ROOM)
	{
		StartItemSpawn();
	}
	else if (RoomType == ERoomType::BOSS_ROOM)
	{
		if (UGameAudioSubsystem* AudioSubsystem =
			GetGameInstance()->GetSubsystem<UGameAudioSubsystem>())
		{
			AudioSubsystem->PlayBossBGM();
		}

		StartBattle();
	}
}

// 敵に関するイベントを受け取り、関連状態を更新する。
void ARoom::HandleEnemyDied(AEnemyOrigin* DeadEnemy)
{
	DeadEnemy->OnEnemyDeath.RemoveDynamic(this, &ARoom::HandleEnemyDied);

	AliveEnemyCount = FMath::Max(AliveEnemyCount - 1, 0);

	if (AliveEnemyCount == 0)
	{
		EndBattle();
	}
}

// 敵に関するイベントを受け取り、関連状態を更新する。
void ARoom::HandleEnemySpawned(AEnemySpawner* EnemySpawner)
{
	AliveEnemyCount += EnemySpawner->GetSpawnedEnemies().Num();

	for (AEnemyOrigin* Enemy : EnemySpawner->GetSpawnedEnemies())
	{
		if (Enemy)
		{
			SpawnedEnemyList.Add(Enemy);
			Enemy->OnEnemyDeath.AddDynamic(this, &ARoom::HandleEnemyDied);
		}
	}
}

// 部屋を戦闘状態へ切り替え、出口制御と敵の攻撃管理を開始する。
void ARoom::StartBattle()
{
	if (bBattleStarted || bBattleCleared)
	{
		return;
	}

	bBattleStarted = true;
	AliveEnemyCount = 0;

	SetDisabledWallsEnable();

	for (auto Spawner : EnemySpawners)
	{
		if (IsValid(Spawner))
		{
			Spawner->StartSpawn();
			Spawner->OnEnemySpawned.AddDynamic(this, &ARoom::HandleEnemySpawned);
		}
	}
}

// 部屋内の敵全滅を確認した後、戦闘を終了して出口と報酬処理を有効にする。
void ARoom::EndBattle()
{
	bBattleCleared = true;
	SetEnabledWallsDisable();

	for (auto Spawner : EnemySpawners)
	{
		if (IsValid(Spawner))
		{
			Spawner->OnEnemySpawned.RemoveDynamic(this, &ARoom::HandleEnemySpawned);
		}
	}
}

// 敵攻撃を取得し、呼び出し元へ返す。
void ARoom::GetEnemyGrantAttack()
{
	CheckAttackableEnemy();

	for (AEnemyOrigin* Enemy : SpawnedEnemyList)
	{
		if (!IsValid(Enemy))
		{
			continue;
		}

		AAIController* Controller =
			Cast<AAIController>(Enemy->GetController());

		if (!IsValid(Controller))
		{
			continue;
		}

		UBlackboardComponent* Blackboard =
			Controller->GetBlackboardComponent();

		if (!IsValid(Blackboard))
		{
			continue;
		}

		const bool bPlayerInAtkRange =
			Blackboard->GetValueAsBool(
				TEXT("isPlayerinAtkRange")
			);

		if (bPlayerInAtkRange &&
			!Enemy->GetIsStunned())
		{
			EnemyInAtkRangeList.AddUnique(Enemy);
		}
		else
		{
			EnemyInAtkRangeList.Remove(Enemy);
		}
	}
}

// 敵攻撃を指定された値へ更新する。
void ARoom::SetEnemyGrantAttack()
{
	CheckAttackableEnemy();

	for (int i = 0; i < EnemyInAtkRangeList.Num() - 1; i++)
	{
		if (EnemyInAtkRangeList[i] == nullptr)
		{
			return;
		}

		if(EnemyInAtkRangeList[i]->GetIsAlive() == false)
		{
			continue;
		}

		if (EnemyInAtkRangeList[i]->GetIsStunned() == true)
		{
			EnemyInAtkRangeList.RemoveAt(i);
			continue;
		}

		AAIController* const cont = Cast<AAIController>(EnemyInAtkRangeList[i]->GetController());

		if (cont != nullptr && cont->GetBlackboardComponent() != nullptr &&
			cont->GetBlackboardComponent()->GetValueAsBool("IsAttackGranted") == true)
		{
			return;
		}
	}

	int RandomRange = FMath::RandRange(0, EnemyInAtkRangeList.Num());

	if (RandomRange < EnemyInAtkRangeList.Num() - 1)
	{
		if (EnemyInAtkRangeList[RandomRange] != nullptr &&
			EnemyInAtkRangeList[RandomRange]->GetController() != nullptr)
		{
			AAIController* const cont = Cast<AAIController>(EnemyInAtkRangeList[RandomRange]->GetController());

			if (cont != nullptr && cont->GetBlackboardComponent() != nullptr)
			{
				cont->GetBlackboardComponent()->SetValueAsBool("IsAttackGranted", true);
			}
		}
	}
}

// 敵の成立条件を確認し、必要な後続処理を行う。
void ARoom::CheckAttackableEnemy()
{
	for (int32 i = SpawnedEnemyList.Num() - 1; i >= 0; --i)
	{
		if (!IsValid(SpawnedEnemyList[i]) ||
			!SpawnedEnemyList[i]->GetIsAlive())
		{
			SpawnedEnemyList.RemoveAt(i);
		}
	}

	for (int32 i = EnemyInAtkRangeList.Num() - 1; i >= 0; --i)
	{
		if (!IsValid(EnemyInAtkRangeList[i]) ||
			!EnemyInAtkRangeList[i]->GetIsAlive())
		{
			EnemyInAtkRangeList.RemoveAt(i);
		}
	}
}

// アイテム生成を開始し、必要な状態とタイマーを有効にする。
void ARoom::StartItemSpawn()
{
	if (bItemSpawned)
	{
		return;
	}

	bItemSpawned = true;

	for (AItemSpawner* Spawner : ItemSpawners)
	{
		if (!IsValid(Spawner))
		{
			continue;
		}

		AItem* Item = Spawner->SpawnItem();

		if (IsValid(Item))
		{
			SpawnedItemList.Add(Item);
		}
	}
}
