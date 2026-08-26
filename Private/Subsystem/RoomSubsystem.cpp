// ゲーム全体で共有する機能を管理する。

#include "Subsystem/RoomSubsystem.h"
#include "../../Room.h"
#include "../../RoomSpawner.h"

// 部屋を管理対象へ追加し、関連状態を更新する。
void URoomSubsystem::AddRoom(const FIntPoint& RoomCoordinates, ARoom* Room)
{
	if (Room)
	{
		MapRooms.Add(RoomCoordinates, Room);
	}
}

// 部屋を取得し、呼び出し元へ返す。
ARoom* URoomSubsystem::GetRoom(const FIntPoint& RoomCoordinates) const
{
	auto RoomPtr = MapRooms.Find(RoomCoordinates);
	if (RoomPtr)
	{
		return *RoomPtr;
	}
	return nullptr;
}

// 部屋の条件を満たしているか判定する。
bool URoomSubsystem::HasRoom(const FVector& RoomLocation) const
{
	for(const auto& Elem : MapRooms)
	{
		if(Elem.Value && Elem.Value->GetActorLocation() == RoomLocation)
		{
			return true;
		}
	}
	return false;
}

// 部屋を取得し、呼び出し元へ返す。
int32 URoomSubsystem::GetAllRoomsCnt() const
{
	return MapRooms.Num();
}

// 部屋を取得し、呼び出し元へ返す。
TSubclassOf<ARoom> URoomSubsystem::GetRoomTemplate(const int32 Index)
{
	TSubclassOf<ARoom> RoomTemplate = nullptr;

	switch (Index)
	{
		case 0:
		if (CombatRoomTemplate.Num() > 0)
		{
			int32 RandomIndex = FMath::RandRange(0, CombatRoomTemplate.Num() - 1);
			CombatRoomTemplate[RandomIndex]->GetDefaultObject<ARoom>()->SetRoomType(ERoomType::COMBAT_ROOM);
			RoomTemplate = CombatRoomTemplate[RandomIndex];
		}
		break;
		case 1:
		{
			if (ItemRoomTemplate.Num() > 0)
			{
				int32 RandomIndex = FMath::RandRange(0, ItemRoomTemplate.Num() - 1);
				ItemRoomTemplate[0]->GetDefaultObject<ARoom>()->SetRoomType(ERoomType::ITEM_ROOM);
				RoomTemplate = ItemRoomTemplate[0];
			}
		}
		break;
		case 2:
		{
			if (TrapRoomTemplate.Num() > 0)
			{
				int32 RandomIndex = FMath::RandRange(0, TrapRoomTemplate.Num() - 1);
				TrapRoomTemplate[0]->GetDefaultObject<ARoom>()->SetRoomType(ERoomType::TRAP_ROOM);
				RoomTemplate = TrapRoomTemplate[0];
			}
		}
		break;
		case 3:
		{
			if (BossRoomTemplate.Num() > 0)
			{
				int32 RandomIndex = FMath::RandRange(0, BossRoomTemplate.Num() - 1);
				BossRoomTemplate[0]->GetDefaultObject<ARoom>()->SetRoomType(ERoomType::BOSS_ROOM);
				RoomTemplate = BossRoomTemplate[0];
			}
		}
		break;
		default:
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid Room Template Index: %d"), Index);
			RoomTemplate = nullptr;
		}
		break;
	}
	return RoomTemplate;
}

// 位置を生成し、位置・所有者・初期状態を設定する。
void URoomSubsystem::GenerateGridLocation(FIntPoint Location, int32 count)
{
	if (count >= MaxRoomCnt)
	{
		CreateRooms();
		return;
	}

	TArray<FIntPoint> tempArr;
	tempArr.Add(FIntPoint(Location.X - 1, Location.Y));
	tempArr.Add(FIntPoint(Location.X + 1, Location.Y));
	tempArr.Add(FIntPoint(Location.X, Location.Y - 1));
	tempArr.Add(FIntPoint(Location.X, Location.Y + 1));

	for (int i = 0; i < SpawnedRoomsLocations.Num(); i++)
	{
		if (FIntPoint(Location.X - 1, Location.Y) == SpawnedRoomsLocations[i])
		{
			tempArr.Remove(FIntPoint(Location.X - 1, Location.Y));
		}
		if (FIntPoint(Location.X + 1, Location.Y) == SpawnedRoomsLocations[i])
		{
			tempArr.Remove(FIntPoint(Location.X + 1, Location.Y));
		}
		if (FIntPoint(Location.X, Location.Y - 1) == SpawnedRoomsLocations[i])
		{
			tempArr.Remove(FIntPoint(Location.X, Location.Y - 1));
		}
		if (FIntPoint(Location.X, Location.Y + 1) == SpawnedRoomsLocations[i])
		{
			tempArr.Remove(FIntPoint(Location.X, Location.Y + 1));
		}
	}

	FIntPoint temp = FIntPoint(0, 0);

	int32 RandomIndex = FMath::RandRange(0, tempArr.Num() - 1);

	temp = tempArr[RandomIndex];

	SpawnedNeighborRooms.Add(FRoomData{ Location, temp });

	SpawnedRoomsLocations.Add(temp);

	GenerateGridLocation(temp, ++count);
}

// 部屋を生成し、位置・所有者・初期状態を設定する。
void URoomSubsystem::CreateRooms()
{
	for (int i = 0; i < SpawnedRoomsLocations.Num(); i++)
	{
		TSubclassOf<ARoom> RoomTemplate = nullptr;

		if (i >= SpawnedRoomsLocations.Num() - 1)
		{
			RoomTemplate = GetRoomTemplate((int32)ERoomType::BOSS_ROOM);
		}
		else if (i < SpawnedRoomsLocations.Num() - 1 && i % 4 == 0)
		{
			RoomTemplate = GetRoomTemplate((int32)ERoomType::ITEM_ROOM);
		}
		else
		{
			RoomTemplate = GetRoomTemplate((int32)ERoomType::COMBAT_ROOM);
		}

		if (GetWorld() != nullptr)
		{
			ARoom* Room = GetWorld()->SpawnActor<ARoom>(
				RoomTemplate,
				FVector(3000.f * SpawnedRoomsLocations[i].X, 3000.f * SpawnedRoomsLocations[i].Y, 0.f),
				FRotator::ZeroRotator
			);

			if(i == 0)
			{
				Room->SpawnPlayer();
			}

			AddRoom(SpawnedRoomsLocations[i], Room);
		}

		if (i > 0)
		{
			CheckNeighborRooms(SpawnedNeighborRooms[i - 1].RoomCoordinates, SpawnedNeighborRooms[i - 1].NeighborCoordinates);
		}
	}
}

// 部屋の成立条件を確認し、必要な後続処理を行う。
void URoomSubsystem::CheckNeighborRooms(const FIntPoint& RoomCoordinates, const FIntPoint& NeighborCoordinates)
{
	ARoom* Room = GetRoom(RoomCoordinates);
	ARoom* NeighborRoom = GetRoom(NeighborCoordinates);

	if (!IsValid(Room))
	{
		return;
	}

	if (!IsValid(NeighborRoom))
	{
		return;
	}

	if (Room && NeighborRoom)
	{

		if (FMath::Abs(RoomCoordinates.X - NeighborCoordinates.X) == 1 && RoomCoordinates.Y == NeighborCoordinates.Y)
		{
			UE_LOG(LogTemp, Warning, TEXT("Room at %s is a neighbor of Room at %s"),
				*RoomCoordinates.ToString(), *NeighborCoordinates.ToString());

			if (RoomCoordinates.X > NeighborCoordinates.X)
			{
				Room->GetLeftMiddleWall()->SetVisibility(false);
				Room->GetLeftMiddleWall()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				NeighborRoom->GetRightMiddleWall()->SetVisibility(false);
				NeighborRoom->GetRightMiddleWall()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
			else
			{
				Room->GetRightMiddleWall()->SetVisibility(false);
				Room->GetRightMiddleWall()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				NeighborRoom->GetLeftMiddleWall()->SetVisibility(false);
				NeighborRoom->GetLeftMiddleWall()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
		else if (FMath::Abs(RoomCoordinates.Y - NeighborCoordinates.Y) == 1 && RoomCoordinates.X == NeighborCoordinates.X)
		{
			UE_LOG(LogTemp, Warning, TEXT("Room at %s is a neighbor of Room at %s"),
				*RoomCoordinates.ToString(), *NeighborCoordinates.ToString());

			if(RoomCoordinates.Y > NeighborCoordinates.Y)
			{
				Room->GetTopMiddleWall()->SetVisibility(false);
				Room->GetTopMiddleWall()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				NeighborRoom->GetBottomMiddleWall()->SetVisibility(false);
				NeighborRoom->GetBottomMiddleWall()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
			else
			{
				Room->GetBottomMiddleWall()->SetVisibility(false);
				Room->GetBottomMiddleWall()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				NeighborRoom->GetTopMiddleWall()->SetVisibility(false);
				NeighborRoom->GetTopMiddleWall()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
}

// 部屋を管理対象から削除し、保持状態を整理する。
void URoomSubsystem::RemoveAllRooms()
{
	for (auto& Elem : MapRooms)
	{
		if (Elem.Value)
		{
			Elem.Value->Destroy();
		}
	}
	MapRooms.Empty();
}

// サブシステムを解除し、再利用可能な初期状態へ戻す。
void URoomSubsystem::ClearSubsystem()
{
	MapRooms.Empty();
	SpawnedRoomsLocations.Empty();
	SpawnedNeighborRooms.Empty();
	CurrLocation = FIntPoint(0, 0);
}
