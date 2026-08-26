// 画面表示と操作を処理する。

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"


class UButton;
class UBorder;

// 画面表示と操作を処理する役割を持つ。
UCLASS()
class PJ26_API UGameOverWidget : public UUserWidget
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
    TObjectPtr<UButton> RestartButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> QuitButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UBorder> RestartBorder;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UBorder> QuitBorder;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    TObjectPtr<UWidgetAnimation> FadeOutAnimation;

public:
    // 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
    void FocusButton();

    // 対象を指定された値へ更新する。
    void SetSelectedBorder(UButton* FocusedButton);

private:
    // 対象が選択された際に、対応する画面遷移または操作を実行する。
    UFUNCTION()
    void OnRestartClicked();

    // 対象が選択された際に、対応する画面遷移または操作を実行する。
    UFUNCTION()
    void OnQuitClicked();

    // 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
    UFUNCTION()
    void OnRestartHovered();

    // 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
    UFUNCTION()
    void OnQuitHovered();

    // 処理の完了を確認し、後続の状態または画面へ移行する。
    UFUNCTION()
    void OnFadeOutFinished();

private:
    bool IsRestart = false;
    bool IsQuit = false;
};
