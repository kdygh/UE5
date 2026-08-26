// アイテムの取得効果を処理する。

#include "Item/Item.h"
#include "../../MyPlayer.h"
#include "../Public/Data/PlayerItemInfo.h"
#include "../Public/Subsystem/EffectSubsystem.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
AItem::AItem()
{

	PrimaryActorTick.bCanEverTick = true;

}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void AItem::BeginPlay()
{
	Super::BeginPlay();
}


// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// アイテムの種類と設定値に応じて、プレイヤーの能力値を変更する。
void AItem::ApplyEffect(AMyPlayer* player, const EItemType itemType)
{
	switch (itemType)
	{
		case EItemType::RECOVER_HP:
		{
			if (UEffectSubsystem* EffectSubsystem = GetWorld()->GetSubsystem<UEffectSubsystem>())
			{
				EffectSubsystem->SpawnRecoverHPFX(player->GetActorLocation(), FRotator::ZeroRotator);
			}
		}
			break;
		case EItemType::INCREASE_ATKPOWER:
		{
			if (UEffectSubsystem* EffectSubsystem = GetWorld()->GetSubsystem<UEffectSubsystem>())
			{
				EffectSubsystem->SpawnIncreaseAtkPowerFX(player->GetActorLocation(), FRotator::ZeroRotator);
			}
		}
			break;
		case EItemType::INCREASE_DEF:
		{
			if (UEffectSubsystem* EffectSubsystem = GetWorld()->GetSubsystem<UEffectSubsystem>())
			{
				EffectSubsystem->SpawnIncreaseDefFX(player->GetActorLocation(), FRotator::ZeroRotator);
			}
		}
			break;
		case EItemType::INCREASE_SPEED:
		{
			if (UEffectSubsystem* EffectSubsystem = GetWorld()->GetSubsystem<UEffectSubsystem>())
			{
				EffectSubsystem->SpawnIncreaseSpeedFX(player->GetActorLocation(), FRotator::ZeroRotator);
			}
		}
			break;
		case EItemType::INCREASE_MAXHP:
		{
			if (UEffectSubsystem* EffectSubsystem = GetWorld()->GetSubsystem<UEffectSubsystem>())
			{
				EffectSubsystem->SpawnIncreaseMaxHPFX(player->GetActorLocation(), FRotator::ZeroRotator);
			}
		}
			break;
		case EItemType::INCREASE_MAXSP:
		{
			if (UEffectSubsystem* EffectSubsystem = GetWorld()->GetSubsystem<UEffectSubsystem>())
			{
				EffectSubsystem->SpawnIncreaseMaxSPFX(player->GetActorLocation(), FRotator::ZeroRotator);
			}
		}
			break;
		default:
			break;
	}
}

// データ表からアイテム情報を読み込み、名前・説明・適用量を初期化する。
void AItem::InitializeItem(const FName& itemName)
{
	if(ItemInfoTable == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemInfoTable is nullptr"));
		return;
	}

	FPlayerItemInfo* ItemInfo = ItemInfoTable->FindRow<FPlayerItemInfo>(itemName, TEXT(""));
	if(ItemInfo == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemInfo not found for item: %s"), *itemName.ToString());
		return;
	}

	ItemName = ItemInfo->ItemName;
	ItemDesc = ItemInfo->ItemDesc;
	Amount = ItemInfo->Amount;
	ItemClass = ItemInfo->ItemClass;
}
