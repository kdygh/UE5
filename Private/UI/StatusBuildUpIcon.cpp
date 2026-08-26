// 画面表示と操作を処理する。

#include "UI/StatusBuildUpIcon.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Texture2D.h"

// 画面部品の生成後に、子要素・イベント・初期表示を設定する。
void UStatusBuildUpIcon::NativeConstruct()
{
	Super::NativeConstruct();

	if (FillImage && FillMaterial)
	{
		FillMID = UMaterialInstanceDynamic::Create(FillMaterial, this);
		FillImage->SetBrushFromMaterial(FillMID);
	}

	UE_LOG(LogTemp, Warning, TEXT("UStatusBuildUpIcon NativeConstruct %s"), *GetName());
}

// 対象を指定された値へ更新する。
void UStatusBuildUpIcon::SetIcon(UTexture2D* InTexture)
{
	if (BaseIconImage && FillImage && InTexture)
	{
		BaseIconImage->SetBrushFromTexture(InTexture);
		FillImage->SetBrushFromTexture(InTexture);
	}
}

// 進捗を指定された値へ更新する。
void UStatusBuildUpIcon::SetProgress(float InProgress)
{
	if (!FillMID)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ProgressMID is not initialized in DebuffIconWidget"));
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Setting Progress: %f"), InProgress));


	const float Clamped = FMath::Clamp(InProgress, 0.0f, 1.0f);
	FillMID->SetScalarParameterValue(TEXT("Progress"), Clamped);

	GEngine->AddOnScreenDebugMessage(
		-1, 0.f, FColor::Green,
		FString::Printf(TEXT("Setting Progress: %f"), Clamped)
	);
}
