// 画面表示と操作を処理する。

#include "UI/TutorialWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Subsystem/GameFlowSubsystem.h"
#include "Subsystem/GameAudioSubsystem.h"
#include <Kismet/GameplayStatics.h>

// 画面部品の生成後に、子要素・イベント・初期表示を設定する。
void UTutorialWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ResumeButton)
    {
        ResumeButton->OnClicked.AddUniqueDynamic(
            this,
            &UTutorialWidget::OnResumeClicked
        );

        ResumeButton->OnClicked.AddUniqueDynamic(
            this,
            &UTutorialWidget::OnResumeHovered
        );
    }
}

// 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
void UTutorialWidget::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
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

// 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
void UTutorialWidget::FocusButton()
{
    if (ResumeButton && GetOwningPlayer())
    {
        ResumeButton->SetUserFocus(
            GetOwningPlayer()
        );
    }
}

// 選択対象の変化に応じて、フォーカス状態と表示枠を更新する。
void UTutorialWidget::OnResumeHovered()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        ResumeButton->SetUserFocus(PC);
    }
}

// 対象が選択された際に、対応する画面遷移または操作を実行する。
void UTutorialWidget::OnResumeClicked()
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
            GameFlow->HideTutorial();
        }
    }
}

// 対象を現在の実行状態に合わせて再計算し、関連要素へ反映する。
void UTutorialWidget::UpdateButtonBorders()
{
    UE_LOG(LogTemp, Warning, TEXT("UpdateButtonBorders"));
    APlayerController* PC = GetOwningPlayer();

    if (!PC ||
        !ResumeButton ||
        !ResumeBorder)
    {
        return;
    }

    const bool bResumeSelected =
        ResumeButton->HasUserFocus(PC);

    ResumeBorder->SetVisibility(
        bResumeSelected
        ? ESlateVisibility::HitTestInvisible
        : ESlateVisibility::Hidden
    );
}
