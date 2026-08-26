// 敵固有の行動と戦闘を処理する。

#include "UI/Enemy/DebuffIconWidget.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Texture2D.h"


// 画面部品の生成後に、子要素・イベント・初期表示を設定する。
void UDebuffIconWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!DebuffProgressImage)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("DebuffProgressImage is null"));
		return;
	}

	if (!ProgressMaterial)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ProgressMaterial is null"));
		return;
	}

	ProgressMID = UMaterialInstanceDynamic::Create(ProgressMaterial, this);

	if (!ProgressMID)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to create ProgressMID"));
		return;
	}

	DebuffProgressImage->SetBrushFromMaterial(ProgressMID);
}

// 対象を指定された値へ更新する。
void UDebuffIconWidget::SetIcon(UTexture2D* InTexture)
{
	if (DebuffIconImage && InTexture)
	{
		DebuffIconImage->SetBrushFromTexture(InTexture);
	}
}

// 進捗を指定された値へ更新する。
void UDebuffIconWidget::SetProgress(float InProgress)
{
	if (!ProgressMID)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ProgressMID is not initialized in DebuffIconWidget"));
		return;
	}

	const float Clamped = FMath::Clamp(InProgress, 0.0f, 1.0f);
	ProgressMID->SetScalarParameterValue(TEXT("Progress"), Clamped);
}
