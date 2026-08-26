// 画面表示と操作を処理する。

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TutorialWidget.generated.h"


class UButton;
class UBorder;

// 画面表示と操作を処理する役割を持つ。
UCLASS()
class PJ26_API UTutorialWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// 画面部品の生成後に、子要素・イベント・初期表示を設定する。
	virtual void NativeConstruct() override;

	virtual void NativeOnFocusChanging(
		const FWeakWidgetPath& PreviousFocusPath,
		const FWidgetPath& NewWidgetPath,
		const FFocusEvent& InFocusEvent
	) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ResumeButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> ResumeBorder;

	// 対象を現在の実行状態に合わせて再計算し、関連要素へ反映する。
	void UpdateButtonBorders();

public:
	// 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
	UFUNCTION()
	void FocusButton();

	// 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
	UFUNCTION()
	void OnResumeHovered();

private:
	// 対象が選択された際に、対応する画面遷移または操作を実行する。
	UFUNCTION()
	void OnResumeClicked();
};
