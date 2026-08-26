// 敵固有の行動と戦闘を処理する。

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DebuffIconWidget.generated.h"


class UImage;
class UTexture2D;
class UMaterialInstanceDynamic;

// 敵固有の行動と戦闘を処理する役割を持つ。
UCLASS()
class PJ26_API UDebuffIconWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 画面部品の生成後に、子要素・イベント・初期表示を設定する。
	virtual void NativeConstruct() override;

	// 対象を指定された値へ更新する。
	void SetIcon(UTexture2D* InTexture);
	// 進捗を指定された値へ更新する。
	void SetProgress(float InProgress);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> DebuffIconImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> DebuffProgressImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debuff")
	TObjectPtr<UMaterialInterface> ProgressMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ProgressMID;
};
