// 部屋の生成と戦闘進行を管理する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Room.generated.h"

// 部屋の生成と戦闘進行を管理する役割を持つ。
UENUM()
enum class ERoomType
{
	COMBAT_ROOM = 0,
	ITEM_ROOM,
	TRAP_ROOM,
	BOSS_ROOM
};

class UBoxComponent;
class AEnemySpawner;
class AItemSpawner;
class AEnemyOrigin;
class AItem;
class AMyPlayer;

// 部屋の生成と戦闘進行を管理する役割を持つ。
UCLASS()
class PJ26_API ARoom : public AActor
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	ARoom();

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;

	// 構成要素の生成完了後に、相互参照とイベントを接続する。
	virtual void PostInitializeComponents() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room Type")
	FString RoomTypeName;

public:

	// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
	virtual void Tick(float DeltaTime) override;

	// 重複実行を防ぎ、部屋に登録されたすべての敵から状態異常を解除する。
	void CleanRoom();

	void SetRoomDepth(UINT InRoomDepth)
	{
		RoomDepth = InRoomDepth;
	}

	UINT GetRoomDepth() const
	{
		return RoomDepth;
	}

	void SetLocation(const FIntPoint& InLocation)
	{
		Location = InLocation;
	}

	FIntPoint GetLocation() const
	{
		return Location;
	}

	UStaticMeshComponent* GetTopMiddleWall() const
	{
		return TopMiddleWall;
	}

	UStaticMeshComponent* GetBottomMiddleWall() const
	{
		return BottomMiddleWall;
	}

	UStaticMeshComponent* GetLeftMiddleWall() const
	{
		return LeftMiddleWall;
	}

	UStaticMeshComponent* GetRightMiddleWall() const
	{
		return RightMiddleWall;
	}

	void SetIsFirstRoom(bool bInIsFirstRoom)
	{
		IsFirstRoom = bInIsFirstRoom;
	}

	bool GetIsFirstRoom() const
	{
		return IsFirstRoom;
	}

	TArray<UStaticMeshComponent*> GetDisabledWalls() const
	{
		return DisabledWalls;
	}

	// プレイヤーを生成し、位置・所有者・初期状態を設定する。
	void SpawnPlayer();

	// 対象を指定された値へ更新する。
	void SetDisabledWallsEnable();
	// 対象を指定された値へ更新する。
	void SetEnabledWallsDisable();

	UFUNCTION()
	void HandleRoomTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

public:
	// 敵に関するイベントを受け取り、関連状態を更新する。
	UFUNCTION()
	void HandleEnemyDied(AEnemyOrigin* DeadEnemy);

	// 敵に関するイベントを受け取り、関連状態を更新する。
	UFUNCTION()
	void HandleEnemySpawned(AEnemySpawner* EnemySpawner);

	// 部屋を戦闘状態へ切り替え、出口制御と敵の攻撃管理を開始する。
	void StartBattle();
	// 部屋内の敵全滅を確認した後、戦闘を終了して出口と報酬処理を有効にする。
	void EndBattle();
	// 敵攻撃を取得し、呼び出し元へ返す。
	void GetEnemyGrantAttack();
	// 敵攻撃を指定された値へ更新する。
	void SetEnemyGrantAttack();
	// 敵の成立条件を確認し、必要な後続処理を行う。
	void CheckAttackableEnemy();

	// アイテム生成を開始し、必要な状態とタイマーを有効にする。
	void StartItemSpawn();

	void SetRoomType(ERoomType InRoomType)
	{
		RoomType = InRoomType;
	}

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TopMiddleWall;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BottomMiddleWall;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* LeftMiddleWall;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* RightMiddleWall;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<APawn> PlayerClass;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* RoomEnteranceTrigger;

	TArray<UStaticMeshComponent*> DisabledWalls;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room Type", meta = (AllowPrivateAccess = "true"))
	ERoomType RoomType;
	UINT RoomDepth = 0;
	FIntPoint Location;

	bool IsFirstRoom = false;
	bool IsRoomCleaned = false;

	TArray<AEnemySpawner*> EnemySpawners;
	TArray<AItemSpawner*> ItemSpawners;
	TArray<AEnemyOrigin*> SpawnedEnemyList;
	TArray<AEnemyOrigin*> EnemyInAtkRangeList;
	TArray<AItem*> SpawnedItemList;
	AMyPlayer* PlayerInRoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle", meta = (AllowPrivateAccess = "true"))
	int32 AliveEnemyCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle", meta = (AllowPrivateAccess = "true"))
	bool bBattleStarted = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle", meta = (AllowPrivateAccess = "true"))
	bool bBattleCleared = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle", meta = (AllowPrivateAccess = "true"))
	bool bItemSpawned = false;
};
