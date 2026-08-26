// 画面表示と操作を処理する。

#include "UI/Player/PlayerHpWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Engine/Texture2D.h"
#include "../../../MyPlayer.h"
#include <Blueprint/WidgetLayoutLibrary.h>

// 画面部品の生成後に、子要素・イベント・初期表示を設定する。
void UPlayerHpWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateHp();
	UpdateSp();
	UpdateAttackAttributeImage();
}

// ロックオンを使用するプレイヤーを設定する。
void UPlayerHpWidget::SetOwnerPlayer(AMyPlayer* InPlayer)
{
	OwnerPlayer = InPlayer;
	UpdateHp();
	UpdateSp();
	UpdateAttackAttributeImage();
}

// 体力を現在の実行状態に合わせて再計算し、関連要素へ反映する。
void UPlayerHpWidget::UpdateHp()
{
	if (!HPBar || !OwnerPlayer)
	{
		return;
	}

	HPBar->SetPercent(OwnerPlayer->GetHpPercent());
}

// スタミナを現在の実行状態に合わせて再計算し、関連要素へ反映する。
void UPlayerHpWidget::UpdateSp()
{
	if (!SPBar || !OwnerPlayer)
	{
		return;
	}

	SPBar->SetPercent(OwnerPlayer->GetSpPercent());
}

// 攻撃属性を現在の実行状態に合わせて再計算し、関連要素へ反映する。
void UPlayerHpWidget::UpdateAttackAttributeImage()
{
	if (!AttackAttrImage || !OwnerPlayer)
	{
		return;
	}

	UTexture2D* SelectedTexture = DefaultAttrIcon;

	switch (OwnerPlayer->GetCurrentAttackAttribute())
	{
	case EAttackAttribute::Fire:
		SelectedTexture = FireAttrIcon;
		break;

	case EAttackAttribute::Lightning:
		SelectedTexture = LightningAttrIcon;
		break;

	case EAttackAttribute::Wind:
		SelectedTexture = WindAttrIcon;
		break;

	case EAttackAttribute::None:
	default:
		SelectedTexture = DefaultAttrIcon;
		break;
	}

	if (SelectedTexture)
	{
		AttackAttrImage->SetBrushFromTexture(SelectedTexture);
	}
}
