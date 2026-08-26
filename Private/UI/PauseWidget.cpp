// 画面表示と操作を処理する。

#include "UI/PauseWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Subsystem/GameFlowSubsystem.h"
#include <Kismet/GameplayStatics.h>
#include "../../MyPlayer.h"
#include "Subsystem/GameAudioSubsystem.h"

// 画面部品の生成後に、子要素・イベント・初期表示を設定する。
void UPauseWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ResumeButton)
    {
        ResumeButton->OnClicked.AddUniqueDynamic(
            this,
            &UPauseWidget::OnResumeClicked
        );

        ResumeButton->OnHovered.AddUniqueDynamic(
            this,
            &UPauseWidget::OnResumeGameHovered
        );

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("ResumeButton bound: %s"),
            ResumeButton->OnHovered.IsBound()
            ? TEXT("true")
            : TEXT("false")
        );
    }

    if (TutorialButton)
    {
        TutorialButton->OnClicked.AddUniqueDynamic(
            this,
            &UPauseWidget::OnTutorialClicked
        );

        TutorialButton->OnHovered.AddUniqueDynamic(
            this,
            &UPauseWidget::OnTutorialHovered
        );
    }

    if (RestartButton)
    {
        RestartButton->OnClicked.AddUniqueDynamic(
            this,
            &UPauseWidget::OnRestartClicked
        );

        RestartButton->OnHovered.AddUniqueDynamic(
            this,
            &UPauseWidget::OnRestartGameHovered
        );
    }

    if (QuitButton)
    {
        QuitButton->OnClicked.AddUniqueDynamic(
            this,
            &UPauseWidget::OnQuitClicked
        );

        QuitButton->OnHovered.AddUniqueDynamic(
            this,
            &UPauseWidget::OnQuitGameHovered
        );
    }

    if (FadeOutAnimation)
    {
        FWidgetAnimationDynamicEvent FinishedEvent;

        FinishedEvent.BindDynamic(
            this,
            &UPauseWidget::OnFadeOutFinished
        );

        BindToAnimationFinished(
            FadeOutAnimation,
            FinishedEvent
        );
    }
}

// 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
void UPauseWidget::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
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

    if (ResumeButton &&
        ResumeButton->GetCachedWidget() == NewFocusedWidget)
    {
        SetSelectedBorder(ResumeButton);
    }
    else if (TutorialButton &&
        TutorialButton->GetCachedWidget() == NewFocusedWidget)
    {
        SetSelectedBorder(TutorialButton);
    }
    else if (RestartButton &&
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

// キー入力を先に受け取り、ゲームパッドまたはキーボード操作に応じてメニュー選択を処理する。
FReply UPauseWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    const FKey Key = InKeyEvent.GetKey();

    if (Key == EKeys::Tab ||
        Key == EKeys::Gamepad_Special_Right)
    {
        if (UGameInstance* GI = GetGameInstance())
        {
            if (UGameFlowSubsystem* GameFlow =
                GI->GetSubsystem<UGameFlowSubsystem>())
            {
                GameFlow->ResumeGame();
            }
        }

        return FReply::Handled();
    }

    return Super::NativeOnPreviewKeyDown(
        InGeometry,
        InKeyEvent
    );
}

// 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
void UPauseWidget::FocusButton()
{
    if (!ResumeButton)
    {
        return;
    }

    if (APlayerController* PC = GetOwningPlayer())
    {
        ResumeButton->SetUserFocus(PC);
    }
}

// フェードを解除し、再利用可能な初期状態へ戻す。
void UPauseWidget::ResetFadeImageOpacity()
{
    FadeImage->ColorAndOpacity.A = 0.0f;
    FadeImage->SetRenderOpacity(0.0f);
}

// 対象が選択された際に、対応する画面遷移または操作を実行する。
void UPauseWidget::OnResumeClicked()
{
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UGameFlowSubsystem* GameFlow =
            GI->GetSubsystem<UGameFlowSubsystem>())
        {
            if (UGameAudioSubsystem* AudioSubsystem =
                GetGameInstance()->GetSubsystem<UGameAudioSubsystem>())
            {
                AudioSubsystem->PlayButtonSelectSound();
            }

            if (UGameAudioSubsystem* AudioSubsystem =
                GetGameInstance()->GetSubsystem<UGameAudioSubsystem>())
            {
                AudioSubsystem->PlayButtonSelectSound();
            }
            GameFlow->ResumeGame();
        }
    }
}

// 対象が選択された際に、対応する画面遷移または操作を実行する。
void UPauseWidget::OnTutorialClicked()
{
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UGameAudioSubsystem* AudioSubsystem =
            GetGameInstance()->GetSubsystem<UGameAudioSubsystem>())
        {
            AudioSubsystem->PlayButtonSelectSound();
        }

        if (UGameFlowSubsystem* GameFlow =
            GI->GetSubsystem<UGameFlowSubsystem>())
        {
            GameFlow->ShowTutorial();
        }
    }
}

// 対象が選択された際に、対応する画面遷移または操作を実行する。
void UPauseWidget::OnRestartClicked()
{

    if (RestartButton)
    {
        RestartButton->SetIsEnabled(false);
    }

    if (QuitButton)
    {
        QuitButton->SetIsEnabled(false);
    }

    if (ResumeButton)
    {
        ResumeButton->SetIsEnabled(false);
    }

    if (TutorialButton)
    {
        TutorialButton->SetIsEnabled(false);
    }

    IsRestart = true;

    UWorld* World = GetWorld();

    if (!World)
    {
        return;
    }

    AMyPlayer* Player = Cast<AMyPlayer>(World->GetFirstPlayerController()->GetPawn());

    if (Player)
    {
        Player->GiveUp();
    }

    if (UGameAudioSubsystem* AudioSubsystem =
        GetGameInstance()->GetSubsystem<UGameAudioSubsystem>())
    {
        AudioSubsystem->PlayButtonSelectSound();
    }

    UGameplayStatics::SetGamePaused(World, false);

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
void UPauseWidget::OnQuitClicked()
{

    if (QuitButton)
    {
        QuitButton->SetIsEnabled(false);
    }


    if (RestartButton)
    {
        RestartButton->SetIsEnabled(false);
    }

    if (ResumeButton)
    {
        ResumeButton->SetIsEnabled(false);
    }

    if (TutorialButton)
    {
        TutorialButton->SetIsEnabled(false);
    }

    IsQuit = true;

    if (UGameAudioSubsystem* AudioSubsystem =
        GetGameInstance()->GetSubsystem<UGameAudioSubsystem>())
    {
        AudioSubsystem->PlayButtonSelectSound();
    }

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
void UPauseWidget::OnFadeOutFinished()
{
    if (IsRestart == true)
    {
        if (UGameInstance* GI = GetGameInstance())
        {
            if (UGameAudioSubsystem* AudioSubsystem =
                GetGameInstance()->GetSubsystem<UGameAudioSubsystem>())
            {
                AudioSubsystem->PlayButtonSelectSound();
            }

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
            if (UGameAudioSubsystem* AudioSubsystem =
                GetGameInstance()->GetSubsystem<UGameAudioSubsystem>())
            {
                AudioSubsystem->PlayButtonSelectSound();
            }

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
void UPauseWidget::OnTutorialHovered()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        TutorialButton->SetUserFocus(PC);
        SetSelectedBorder(TutorialButton);
    }
}

// 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
void UPauseWidget::OnResumeGameHovered()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        ResumeButton->SetUserFocus(PC);
        SetSelectedBorder(ResumeButton);
    }
}

// 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
void UPauseWidget::OnRestartGameHovered()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        RestartButton->SetUserFocus(PC);
        SetSelectedBorder(RestartButton);
    }
}

// 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
void UPauseWidget::OnQuitGameHovered()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        QuitButton->SetUserFocus(PC);
        SetSelectedBorder(QuitButton);
    }
}

// 対象を現在の実行状態に合わせて再計算し、関連要素へ反映する。
void UPauseWidget::UpdateButtonBorders()
{
    UE_LOG(LogTemp, Warning, TEXT("UpdateButtonBorders"));
    APlayerController* PC = GetOwningPlayer();

    if (!PC ||
        !TutorialButton ||
        !ResumeButton ||
        !RestartButton ||
        !QuitButton ||
        !TutorialBorder ||
        !ResumeBorder ||
        !RestartBorder ||
        !QuitBorder)
    {
        return;
    }

    const bool bTutorialSelected =
        TutorialButton->HasUserFocus(PC);

    const bool bResumeSelected =
        ResumeButton->HasUserFocus(PC);

    const bool bRestartSelected =
        RestartButton->HasUserFocus(PC);

    const bool bQuitSelected =
        QuitButton->HasUserFocus(PC);

    TutorialBorder->SetVisibility(
        bTutorialSelected
        ? ESlateVisibility::HitTestInvisible
        : ESlateVisibility::Hidden
    );

    ResumeBorder->SetVisibility(
        bResumeSelected
        ? ESlateVisibility::HitTestInvisible
        : ESlateVisibility::Hidden
    );

    RestartBorder->SetVisibility(
        bRestartSelected
        ? ESlateVisibility::HitTestInvisible
        : ESlateVisibility::Hidden
    );

    QuitBorder->SetVisibility(
        bQuitSelected
        ? ESlateVisibility::HitTestInvisible
        : ESlateVisibility::Hidden
    );
}

// 対象を指定された値へ更新する。
void UPauseWidget::SetSelectedBorder(UButton* FocusedButton)
{
    ResumeBorder->SetVisibility(
        FocusedButton == ResumeButton
        ? ESlateVisibility::HitTestInvisible
        : ESlateVisibility::Hidden
    );

    TutorialBorder->SetVisibility(
        FocusedButton == TutorialButton
        ? ESlateVisibility::HitTestInvisible
        : ESlateVisibility::Hidden
    );

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

// 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
UWidget* UPauseWidget::GetDefaultFocusWidget() const
{
    return ResumeButton;
}
