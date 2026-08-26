// ゲーム内オブジェクトの動作を処理する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawner.generated.h"

class AItem;

// ゲーム内オブジェクトの動作を処理する役割を持つ。
UCLASS()
class PJ26_API AItemSpawner : public AActor
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	AItemSpawner();

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;

public:

	// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
	virtual void Tick(float DeltaTime) override;

	// 候補一覧からアイテムを選択し、指定された位置へ生成する。
	AItem* SpawnItem();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Spawner", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AItem> ItemClass;

	bool bSpawnerActive = false;
};
