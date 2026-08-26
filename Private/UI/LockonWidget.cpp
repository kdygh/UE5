// 画面表示と操作を処理する。

#include "UI/LockonWidget.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Engine/Texture2D.h"
#include <Blueprint/WidgetLayoutLibrary.h>

// 画面部品の生成後に、子要素・イベント・初期表示を設定する。
void ULockonWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

// ロックオンを現在の実行状態に合わせて再計算し、関連要素へ反映する。
void ULockonWidget::UpdateLockOnMarker(AActor* TargetActor)
{
	if (!LockOnMarker)
	{
		return;
	}

	if (!TargetActor)
	{
		LockOnMarker->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	APlayerController* PC = GetOwningPlayer();

	if (!PC)
	{
		return;
	}

	FVector TargetLocation =
		TargetActor->GetActorLocation();

	FVector2D ScreenPosition;

	const bool bProjected =
		UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
			PC,
			TargetLocation,
			ScreenPosition,
			false
		);

	if (!bProjected)
	{
		LockOnMarker->SetVisibility(
			ESlateVisibility::Hidden
		);

		return;
	}

	LockOnMarker->SetVisibility(
		ESlateVisibility::HitTestInvisible
	);

	if (UCanvasPanelSlot* CanvasSlot =
		Cast<UCanvasPanelSlot>(
			LockOnMarker->Slot
		))
	{
		CanvasSlot->SetPosition(ScreenPosition);
	}
}

// ロックオンを非表示にし、入力や更新の対象から外す。
void ULockonWidget::HideLockOnMarker()
{
	if (LockOnMarker)
	{
		LockOnMarker->SetVisibility(
			ESlateVisibility::Hidden
		);
	}
}
