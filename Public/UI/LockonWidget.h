// 画面表示と操作を処理する。

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LockonWidget.generated.h"


class UImage;

// 画面表示と操作を処理する役割を持つ。
UCLASS()
class PJ26_API ULockonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 画面部品の生成後に、子要素・イベント・初期表示を設定する。
	virtual void NativeConstruct() override;

	// ロックオンを現在の実行状態に合わせて再計算し、関連要素へ反映する。
	void UpdateLockOnMarker(AActor* TargetActor);
	// ロックオンを非表示にし、入力や更新の対象から外す。
	void HideLockOnMarker();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> LockOnMarker;
};
