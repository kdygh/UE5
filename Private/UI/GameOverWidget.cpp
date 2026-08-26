// 画面表示と操作を処理する。

#include "UI/GameOverWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Subsystem/GameFlowSubsystem.h"

// 画面部品の生成後に、子要素・イベント・初期表示を設定する。
void UGameOverWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (RestartButton)
    {
        RestartButton->OnClicked.AddUniqueDynamic(
            this,
            &UGameOverWidget::OnRestartClicked
        );

        RestartButton->OnHovered.AddUniqueDynamic(
            this,
            &UGameOverWidget::OnRestartHovered
        );
    }

    if (QuitButton)
    {
        QuitButton->OnClicked.AddUniqueDynamic(
            this,
            &UGameOverWidget::OnQuitClicked
        );

        QuitButton->OnHovered.AddUniqueDynamic(
            this,
            &UGameOverWidget::OnQuitHovered
        );
    }

    if (FadeOutAnimation)
    {
        FWidgetAnimationDynamicEvent FinishedEvent;

        FinishedEvent.BindDynamic(
            this,
            &UGameOverWidget::OnFadeOutFinished
        );

        BindToAnimationFinished(
            FadeOutAnimation,
            FinishedEvent
        );
    }
}

// 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
void UGameOverWidget::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
    Super::NativeOnFocusChanging(
        PreviousFocusPath,
        NewWidgetPath,
        InFocusEvent
    );

    if (!NewWidgetPath.IsValid())
    {
        return;
    }

    TSharedRef<SWidget> NewFocusedWidget =
        NewWidgetPath.GetLastWidget();

    if (RestartButton &&
        RestartButton->GetCachedWidget() == NewFocusedWidget)
        {
            SetSelectedBorder(RestartButton);
        }
    else if (QuitButton &&
        QuitButton->GetCachedWidget() == NewFocusedWidget)
        {
            SetSelectedBorder(QuitButton);
        }
}

// 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
void UGameOverWidget::FocusButton()
{
    if (!RestartButton)
    {
        return;
    }

    if (APlayerController* PC = GetOwningPlayer())
    {
        RestartButton->SetUserFocus(PC);
    }
}

// 対象が選択された際に、対応する画面遷移または操作を実行する。
void UGameOverWidget::OnRestartClicked()
{

    if (RestartButton)
    {
        RestartButton->SetIsEnabled(false);
    }

    if (QuitButton)
    {
        QuitButton->SetIsEnabled(false);
    }

    IsRestart = true;

    if (FadeOutAnimation)
    {
        PlayAnimation(FadeOutAnimation);
    }
    else
    {

        OnFadeOutFinished();
    }
}

// 対象が選択された際に、対応する画面遷移または操作を実行する。
void UGameOverWidget::OnQuitClicked()
{

    if (QuitButton)
    {
        QuitButton->SetIsEnabled(false);
    }


    if (RestartButton)
    {
        RestartButton->SetIsEnabled(false);
    }

    IsQuit = true;

    if (FadeOutAnimation)
    {
        PlayAnimation(FadeOutAnimation);
    }
    else
    {

        OnFadeOutFinished();
    }
}

// 処理の完了を確認し、後続の状態または画面へ移行する。
void UGameOverWidget::OnFadeOutFinished()
{
    if (IsRestart == true)
    {
        if (UGameInstance* GI = GetGameInstance())
        {
            if (UGameFlowSubsystem* GameFlow =
                GI->GetSubsystem<UGameFlowSubsystem>())
            {
                GameFlow->ResetWidgets();
                GameFlow->StartNewRun(FName("LoadingLevel"));
            }
        }
    }
    else if (IsQuit == true)
    {
        if (UGameInstance* GI = GetGameInstance())
        {
            if (UGameFlowSubsystem* GameFlow =
                GI->GetSubsystem<UGameFlowSubsystem>())
            {
                GameFlow->ResetWidgets();
                GameFlow->QuitGame();
            }
        }
    }
}

// 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
void UGameOverWidget::OnRestartHovered()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        RestartButton->SetUserFocus(PC);
        SetSelectedBorder(RestartButton);
    }
}

// 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
void UGameOverWidget::OnQuitHovered()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        QuitButton->SetUserFocus(PC);
        SetSelectedBorder(QuitButton);
    }
}

// 対象を指定された値へ更新する。
void UGameOverWidget::SetSelectedBorder(UButton* FocusedButton)
{
    RestartBorder->SetVisibility(
        FocusedButton == RestartButton
        ? ESlateVisibility::HitTestInvisible
        : ESlateVisibility::Hidden
    );

    QuitBorder->SetVisibility(
        FocusedButton == QuitButton
        ? ESlateVisibility::HitTestInvisible
        : ESlateVisibility::Hidden
    );
}
