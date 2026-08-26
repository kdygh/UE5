// ゲーム内オブジェクトの動作を処理する。

#include "Objects/EnemySpawner.h"
#include "EnemyOrigin.h"
#include "../../Room.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
AEnemySpawner::AEnemySpawner()
{

	PrimaryActorTick.bCanEverTick = true;
}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	CurrentSpawnCount = 0;
	bSpawnerActive = false;
}

// 終了時に登録済みイベントやタイマーを解除し、保持参照を整理する。
void AEnemySpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// 設定された敵クラスを指定位置に生成し、部屋との所有関係を設定する。
void AEnemySpawner::SpawnEnemy()
{
	if (EnemyClass)
	{
		AEnemyOrigin* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyOrigin>(EnemyClass, GetActorLocation(), GetActorRotation());
		if (SpawnedEnemy)
		{
			SpawnedEnemies.Add(SpawnedEnemy);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyClass is not set in EnemySpawner."));
	}
}

// 生成数と条件を確認し、次の敵を生成するかタイマーを終了する。
void AEnemySpawner::HandleSpawnTick()
{
	if (!bSpawnerActive)
	{
		return;
	}

	if (CurrentSpawnCount >= SpawnCount)
	{
		StopSpawn();
		return;
	}

	SpawnEnemy();
	CurrentSpawnCount++;
}

// 敵生成用タイマーを開始し、設定された周期で生成処理を予約する。
void AEnemySpawner::StartSpawn()
{
	bSpawnerActive = true;
	CurrentSpawnCount = 0;

	GetWorld()->GetTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&AEnemySpawner::HandleSpawnTick,
		SpawnInterval,
		true
	);
}

// 敵生成用タイマーを解除し、追加生成を停止する。
void AEnemySpawner::StopSpawn()
{
	bSpawnerActive = false;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
	}

	OnEnemySpawned.Broadcast(this);
}
