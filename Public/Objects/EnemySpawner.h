// ゲーム内オブジェクトの動作を処理する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

class AEnemyOrigin;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemySpawnSignature, AEnemySpawner*, EnemySpawner);

// ゲーム内オブジェクトの動作を処理する役割を持つ。
UCLASS()
class PJ26_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	AEnemySpawner();

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
	virtual void Tick(float DeltaTime) override;
	// 設定された敵クラスを指定位置に生成し、部屋との所有関係を設定する。
	void SpawnEnemy();

	// 対象を取得し、呼び出し元へ返す。
	TArray<AEnemyOrigin*> GetSpawnedEnemies() const { return SpawnedEnemies; }

	UPROPERTY(BlueprintAssignable, Category = "Spawn|Event")
	FOnEnemySpawnSignature OnEnemySpawned;

protected:
	// 生成数と条件を確認し、次の敵を生成するかタイマーを終了する。
	void HandleSpawnTick();

public:
	// 敵生成用タイマーを開始し、設定された周期で生成処理を予約する。
	UFUNCTION(BlueprintCallable)
	void StartSpawn();

	// 敵生成用タイマーを解除し、追加生成を停止する。
	UFUNCTION(BlueprintCallable)
	void StopSpawn();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Spawner", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AEnemyOrigin> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Spawner", meta = (AllowPrivateAccess = "true"))
	float SpawnInterval = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Spawner", meta = (AllowPrivateAccess = "true"))
	int32 SpawnCount = 0;

	int32 CurrentSpawnCount = 0;
	bool bSpawnerActive = false;

	FTimerHandle SpawnTimerHandle;
	TArray<AEnemyOrigin*> SpawnedEnemies;
};
