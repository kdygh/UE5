// ゲーム内オブジェクトの動作を処理する。

#include "Objects/ItemSpawner.h"
#include "../Public/Item/Item.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
AItemSpawner::AItemSpawner()
{

	PrimaryActorTick.bCanEverTick = true;

}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void AItemSpawner::BeginPlay()
{
	Super::BeginPlay();

}


// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void AItemSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// 候補一覧からアイテムを選択し、指定された位置へ生成する。
AItem* AItemSpawner::SpawnItem()
{
	if(bSpawnerActive == true)
	{
		return nullptr;
	}

	AItem* SpawnedItem = nullptr;

	if (ItemClass)
	{
		bSpawnerActive = true;
		SpawnedItem = GetWorld()->SpawnActor<AItem>(ItemClass, GetActorLocation(), GetActorRotation());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemClass is not set in ItemSpawner."));
	}

	return SpawnedItem;
}
