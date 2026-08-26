// ゲーム全体で共有する機能を管理する。

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RoomSubsystem.generated.h"


// ゲーム全体で共有する機能を管理する役割を持つ。
USTRUCT()
struct FRoomData
{
	GENERATED_BODY()
	UPROPERTY()
	FIntPoint RoomCoordinates;
	UPROPERTY()
	FIntPoint NeighborCoordinates;
};

class ARoom;

// ゲーム全体で共有する機能を管理する役割を持つ。
UCLASS()
class PJ26_API URoomSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// 部屋を管理対象へ追加し、関連状態を更新する。
	void AddRoom(const FIntPoint& RoomCoordinates, ARoom* Room);
	// 部屋を取得し、呼び出し元へ返す。
	ARoom* GetRoom(const FIntPoint& RoomCoordinates) const;
	// 部屋の条件を満たしているか判定する。
	bool HasRoom(const FVector& RoomLocation) const;
	// 部屋を取得し、呼び出し元へ返す。
	int32 GetAllRoomsCnt() const;
	void SetCurrentRoom(ARoom* Room)
	{
		CurrentRoom = Room;
	}

	bool IsLocationOccupied(const FIntPoint& Location) const
	{
		for(int32 i = 0; i < SpawnedRoomsLocations.Num(); i++)
		{
			if(SpawnedRoomsLocations[i].X == Location.X && SpawnedRoomsLocations[i].Y == Location.Y)
			{
				return true;
			}
		}
		return false;
	}

public:
	void SetCurrLocation(const FIntPoint& NewLocation)
	{
		CurrLocation = NewLocation;
	}

	void SetCombatRoomClass(TArray<TSubclassOf<ARoom>> RoomClass)
	{
		CombatRoomTemplate = RoomClass;
	}

	void SetItemRoomClass(TArray<TSubclassOf<ARoom>> RoomClass)
	{
		ItemRoomTemplate = RoomClass;
	}

	void SetTrapRoomClass(TArray<TSubclassOf<ARoom>> RoomClass)
	{
		TrapRoomTemplate = RoomClass;
	}

	void SetBossRoomClass(TArray<TSubclassOf<ARoom>> RoomClass)
	{
		BossRoomTemplate = RoomClass;
	}

	TArray<FIntPoint> GetSpawnedRoomsLocations() const
	{
		return SpawnedRoomsLocations;
	}

	void AddSpawnedRoomLocation(const FIntPoint& Location)
	{
		SpawnedRoomsLocations.Add(Location);
	}

	FIntPoint GetCurrLocation() const
	{
		return CurrLocation;
	}

	int32 GetMaxRoomCnt() const
	{
		return MaxRoomCnt;
	}

	// 部屋を取得し、呼び出し元へ返す。
	TSubclassOf<ARoom> GetRoomTemplate(const int32 Index);

	// 位置を生成し、位置・所有者・初期状態を設定する。
	void GenerateGridLocation(FIntPoint Location, int32 count);

	// 部屋を生成し、位置・所有者・初期状態を設定する。
	void CreateRooms();
	// 部屋の成立条件を確認し、必要な後続処理を行う。
	void CheckNeighborRooms(const FIntPoint& RoomCoordinates, const FIntPoint& NeighborCoordinates);

public:
	// 部屋を管理対象から削除し、保持状態を整理する。
	void RemoveAllRooms();
	// サブシステムを解除し、再利用可能な初期状態へ戻す。
	void ClearSubsystem();

protected:
	UPROPERTY()
	TMap<FIntPoint, ARoom*> MapRooms;

	UPROPERTY()
	TArray<FIntPoint> SpawnedRoomsLocations;

	UPROPERTY()
	TArray<FRoomData> SpawnedNeighborRooms;

private:
	FIntPoint CurrLocation = FIntPoint(0, 0);

	TArray<TSubclassOf<ARoom>> CombatRoomTemplate;
	TArray<TSubclassOf<ARoom>> ItemRoomTemplate;
	TArray<TSubclassOf<ARoom>> TrapRoomTemplate;
	TArray<TSubclassOf<ARoom>> BossRoomTemplate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	int32 MaxRoomCnt = 8;

	ARoom* CurrentRoom = nullptr;
};
