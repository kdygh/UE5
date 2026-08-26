// 画面表示と操作を処理する。

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseWidget.generated.h"


class UButton;
class UBorder;
class UImage;

// 画面表示と操作を処理する役割を持つ。
UCLASS()
class PJ26_API UPauseWidget : public UUserWidget
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

    virtual FReply NativeOnPreviewKeyDown(
        const FGeometry& InGeometry,
        const FKeyEvent& InKeyEvent
    ) override;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> ResumeButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UBorder> ResumeBorder;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> TutorialButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UBorder> TutorialBorder;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> RestartButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UBorder> RestartBorder;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> QuitButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UBorder> QuitBorder;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> FadeImage;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    TObjectPtr<UWidgetAnimation> FadeOutAnimation;

public:
    // 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
    void FocusButton();

    // フェードを解除し、再利用可能な初期状態へ戻す。
    void ResetFadeImageOpacity();

    // 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
    UFUNCTION()
    void OnTutorialHovered();

    // 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
    UFUNCTION()
    void OnResumeGameHovered();

    // 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
    UFUNCTION()
    void OnRestartGameHovered();

    // 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
    UFUNCTION()
    void OnQuitGameHovered();

    // 対象を現在の実行状態に合わせて再計算し、関連要素へ反映する。
    void UpdateButtonBorders();

    // 対象を指定された値へ更新する。
    void SetSelectedBorder(UButton* FocusedButton);

    // 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
    UWidget* GetDefaultFocusWidget() const;

private:
    // 対象が選択された際に、対応する画面遷移または操作を実行する。
    UFUNCTION()
    void OnResumeClicked();

    // 対象が選択された際に、対応する画面遷移または操作を実行する。
    UFUNCTION()
    void OnTutorialClicked();

    // 対象が選択された際に、対応する画面遷移または操作を実行する。
    UFUNCTION()
    void OnRestartClicked();

    // 対象が選択された際に、対応する画面遷移または操作を実行する。
    UFUNCTION()
    void OnQuitClicked();

    // 処理の完了を確認し、後続の状態または画面へ移行する。
    UFUNCTION()
    void OnFadeOutFinished();

private:
    bool IsRestart = false;
    bool IsQuit = false;
};
