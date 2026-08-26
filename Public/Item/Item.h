// アイテムの取得効果を処理する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

// アイテムの取得効果を処理する役割を持つ。
UENUM()
enum class EItemType : uint8
{
	RECOVER_HP = 0 UMETA(DisplayName = "RECOVER_HP"),
	INCREASE_ATKPOWER UMETA(DisplayName = "INCREASE_ATKPOWER"),
	INCREASE_DEF UMETA(DisplayName = "INCREASE_DEF"),
	INCREASE_MAXHP UMETA(DisplayName = "INCREASE_MAXHP"),
	INCREASE_MAXSP UMETA(DisplayName = "INCREASE_MAXSP"),
	INCREASE_SPEED UMETA(DisplayName = "INCREASE_SPEED")
};

class AMyPlayer;
class USphereComponent;
class UNiagaraComponent;
class UItemNotificationPanel;

// アイテムの取得効果を処理する役割を持つ。
UCLASS()
class PJ26_API AItem : public AActor
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	AItem();

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;

public:

	// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
	virtual void Tick(float DeltaTime) override;

public:
	// アイテムの種類と設定値に応じて、プレイヤーの能力値を変更する。
	virtual void ApplyEffect(AMyPlayer* player, const EItemType itemType);
	// データ表からアイテム情報を読み込み、名前・説明・適用量を初期化する。
	virtual void InitializeItem(const FName& itemName);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collider", meta = (AllowPrivateAccess = "true"))
	USphereComponent* SphereItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* ItemShape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemType", meta = (AllowPrivateAccess = "true"))
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> ItemInfoTable;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString ItemDesc;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Amount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AItem> ItemClass;

};
