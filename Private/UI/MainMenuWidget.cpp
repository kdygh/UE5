// 画面表示と操作を処理する。

#include "UI/MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Subsystem/GameFlowSubsystem.h"
#include "Subsystem/GameAudioSubsystem.h"
#include <Kismet/GameplayStatics.h>
#include "TimerManager.h"


// 画面部品の生成後に、子要素・イベント・初期表示を設定する。
void UMainMenuWidget::NativeConstruct()
{
    if (TutorialButton)
    {
        TutorialButton->OnClicked.AddUniqueDynamic(
            this,
            &UMainMenuWidget::OnTutorialClicked
        );

        TutorialButton->OnHovered.AddUniqueDynamic(
            this,
            &UMainMenuWidget::OnTutorialHovered
        );
    }

    if (StartButton)
    {
        StartButton->OnClicked.AddUniqueDynamic(
            this,
            &UMainMenuWidget::OnStartClicked
        );

        StartButton->OnHovered.AddUniqueDynamic(
            this,
            &UMainMenuWidget::OnStartGameHovered
        );
    }

    if (QuitButton)
    {
        QuitButton->OnClicked.AddUniqueDynamic(
            this,
            &UMainMenuWidget::OnQuitClicked
        );

        QuitButton->OnHovered.AddUniqueDynamic(
            this,
            &UMainMenuWidget::OnQuitGameHovered
        );
    }

    if (FadeOutAnimation)
    {
        FWidgetAnimationDynamicEvent FinishedEvent;

        FinishedEvent.BindDynamic(
            this,
            &UMainMenuWidget::OnFadeOutFinished
        );

        BindToAnimationFinished(
            FadeOutAnimation,
            FinishedEvent
        );
    }

    FocusStartButton();
}

// 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
void UMainMenuWidget::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
    Super::NativeOnFocusChanging(
        PreviousFocusPath,
        NewWidgetPath,
        InFocusEvent
    );

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimerForNextTick(
            FTimerDelegate::CreateWeakLambda(
                this,
                [this]()
                {
                    UpdateButtonBorders();
                }
            )
        );
    }
}

// 対象が選択された際に、対応する画面遷移または操作を実行する。
void UMainMenuWidget::OnStartClicked()
{

    if (QuitButton)
    {
        QuitButton->SetIsEnabled(false);
    }


    if (StartButton)
    {
        StartButton->SetIsEnabled(false);
    }

    if (TutorialButton)
    {
        TutorialButton->SetIsEnabled(false);
    }

    IsStart = true;

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

// 対象が選択された際に、対応する画面遷移または操作を実行する。
void UMainMenuWidget::OnTutorialClicked()
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
void UMainMenuWidget::OnQuitClicked()
{

    if (QuitButton)
    {
        QuitButton->SetIsEnabled(false);
    }


    if (StartButton)
    {
        StartButton->SetIsEnabled(false);
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
void UMainMenuWidget::OnFadeOutFinished()
{
    if (IsStart == true)
    {
        if (UGameInstance* GI = GetGameInstance())
        {
            if (UGameFlowSubsystem* GameFlow =
                GI->GetSubsystem<UGameFlowSubsystem>())
            {
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
                GameFlow->QuitGame();
            }
        }
    }
}

// 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
void UMainMenuWidget::FocusStartButton()
{
    if (!StartButton)
    {
        return;
    }

    if (APlayerController* PC = GetOwningPlayer())
    {
        StartButton->SetUserFocus(PC);
    }
}

// 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
void UMainMenuWidget::OnStartGameHovered()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        StartButton->SetUserFocus(PC);
    }
}

// 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
void UMainMenuWidget::OnQuitGameHovered()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        QuitButton->SetUserFocus(PC);
    }
}

// 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
void UMainMenuWidget::OnTutorialHovered()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        TutorialButton->SetUserFocus(PC);
    }
}

// 対象を現在の実行状態に合わせて再計算し、関連要素へ反映する。
void UMainMenuWidget::UpdateButtonBorders()
{
    UE_LOG(LogTemp, Warning, TEXT("UpdateButtonBorders"));
    APlayerController* PC = GetOwningPlayer();

    if (!PC ||
        !StartButton ||
        !TutorialButton ||
        !QuitButton ||
        !StartBorder ||
        !TutorialBorder ||
        !QuitBorder)
    {
        return;
    }

    const bool bStartSelected =
        StartButton->HasUserFocus(PC);

    const bool bTutorialSelected =
        TutorialButton->HasUserFocus(PC);

    const bool bQuitSelected =
        QuitButton->HasUserFocus(PC);

    StartBorder->SetVisibility(
        bStartSelected
        ? ESlateVisibility::HitTestInvisible
        : ESlateVisibility::Hidden
    );

    TutorialBorder->SetVisibility(
        bTutorialSelected
        ? ESlateVisibility::HitTestInvisible
        : ESlateVisibility::Hidden
    );

    QuitBorder->SetVisibility(
        bQuitSelected
        ? ESlateVisibility::HitTestInvisible
        : ESlateVisibility::Hidden
    );
}
