// ゲーム全体で共有する機能を管理する。

#include "Subsystem/GameFlowSubsystem.h"
#include "Subsystem/RoomSubsystem.h"
#include "Subsystem/EffectSubsystem.h"
#include "Subsystem/EffectActorSubsystem.h"
#include "GameMode/MyPj26GameMode.h"
#include "GameMode/IntroGameMode.h"
#include "UI/MainMenuWidget.h"
#include "UI/GameOverWidget.h"
#include "UI/PauseWidget.h"
#include "UI/TutorialWidget.h"
#include "UI/BossClearedWidget.h"
#include <Kismet/GameplayStatics.h>
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Subsystem/GameFlowSettings.h"
#include "Kismet/KismetSystemLibrary.h"
#include "RenderingThread.h"
#include "TimerManager.h"

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void UGameFlowSubsystem::PauseGame()
{
}

// 一時停止画面を閉じ、ゲーム進行とプレイヤー入力を再開する。
void UGameFlowSubsystem::ResumeGame()
{
    HidePause();
}

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void UGameFlowSubsystem::RestartCurrentRoom()
{
}

// 演出用プールと各画面を初期化し、指定または現在のレベルを読み直す。
void UGameFlowSubsystem::StartNewRun(FName levelName)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    UEffectActorSubsystem* EffectActorSubsystem = World->GetSubsystem<UEffectActorSubsystem>();

    if (EffectActorSubsystem)
    {
        EffectActorSubsystem->ClearPool();
    }

    FlushRenderingCommands();

    HideGameOver();
    HidePause();
    HideBossCleared();

    FName LevelName;

    if (levelName.IsEqual(""))
    {
        FName CurrentLevelName(
            *UGameplayStatics::GetCurrentLevelName(
                World,
                true
            ));

       LevelName = CurrentLevelName;
    }
    else
    {
        LevelName = levelName;
    }

    UGameplayStatics::OpenLevel(
        World,
        LevelName
    );
}

// ゲームオーバー画面を読み込んで表示し、入力を画面操作へ切り替える。
void UGameFlowSubsystem::ShowGameOver()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    APlayerController* PC =
        World->GetFirstPlayerController();

    if (!PC || GameOverWidget)
    {
        return;
    }

    const UGameFlowSettings* Settings =
        GetDefault<UGameFlowSettings>();

    if (!Settings)
    {
        return;
    }

    TSubclassOf<UGameOverWidget> WidgetClass =
        Settings->GameOverWidgetClass.LoadSynchronous();

    if (!WidgetClass)
    {
        return;
    }

    GameOverWidget =
        CreateWidget<UGameOverWidget>(
            PC,
            WidgetClass
        );

    if (!GameOverWidget)
    {
        return;
    }

    GameOverWidget->AddToViewport(100);

    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(
        GameOverWidget->TakeWidget()
    );

    PC->SetInputMode(InputMode);
    PC->bShowMouseCursor = true;

    GameOverWidget->FocusButton();
}

// 終了要求を受け取り、動作中のゲームを安全に終了する。
void UGameFlowSubsystem::QuitGame()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    APlayerController* PlayerController =
        World->GetFirstPlayerController();

    UKismetSystemLibrary::QuitGame(
        World,
        PlayerController,
        EQuitPreference::Quit,
        false
    );
}

// メインメニュー画面を必要に応じて生成し、画面操作用の入力状態を設定する。
void UGameFlowSubsystem::ShowMainMenu()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    APlayerController* PC =
        World->GetFirstPlayerController();

    if (!PC)
    {
        return;
    }


    if (!IsValid(MainMenuWidget))
    {
        const UGameFlowSettings* Settings =
            GetDefault<UGameFlowSettings>();

        if (!Settings)
        {
            return;
        }

        TSubclassOf<UMainMenuWidget> WidgetClass =
            Settings->MainMenuWidgetClass.LoadSynchronous();

        if (!WidgetClass)
        {
            return;
        }

        MainMenuWidget =
            CreateWidget<UMainMenuWidget>(
                PC,
                WidgetClass
            );

        if (!MainMenuWidget)
        {
            return;
        }

        MainMenuWidget->AddToViewport();
    }

    MainMenuWidget->SetVisibility(
        ESlateVisibility::Visible
    );

    FInputModeUIOnly InputMode;
    PC->SetInputMode(InputMode);

    PC->bShowMouseCursor = true;

    UMainMenuWidget* Widget = MainMenuWidget;

    World->GetTimerManager().SetTimerForNextTick(
        FTimerDelegate::CreateWeakLambda(
            Widget,
            [Widget]()
            {
                Widget->FocusStartButton();
            }
        )
    );
}

// メインメニュー画面を閉じ、ゲーム操作用の入力状態へ戻す。
void UGameFlowSubsystem::HideMainMenu()
{
    if (MainMenuWidget)
    {
        MainMenuWidget->SetVisibility(
            ESlateVisibility::Collapsed
        );
    }

    if (UWorld* World = GetWorld())
    {
        if (APlayerController* PC =
            World->GetFirstPlayerController())
        {
            PC->SetShowMouseCursor(false);

            FInputModeGameOnly InputMode;
            PC->SetInputMode(InputMode);
        }
    }
}

// ゲームオーバー画面を閉じ、保持参照と入力状態を通常へ戻す。
void UGameFlowSubsystem::HideGameOver()
{
    if (GameOverWidget)
    {
        GameOverWidget->RemoveFromParent();
        GameOverWidget = nullptr;
    }

    if (UWorld* World = GetWorld())
    {
        if (APlayerController* PC =
            World->GetFirstPlayerController())
        {
            PC->SetShowMouseCursor(false);

            FInputModeGameOnly InputMode;
            PC->SetInputMode(InputMode);
        }
    }
}

// チュートリアル画面を生成して表示し、操作対象を画面へ切り替える。
void UGameFlowSubsystem::ShowTutorial()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    APlayerController* PC =
        World->GetFirstPlayerController();

    if (!PC || TutorialWidget)
    {
        return;
    }

    const UGameFlowSettings* Settings =
        GetDefault<UGameFlowSettings>();

    if (!Settings)
    {
        return;
    }

    TSubclassOf<UTutorialWidget> WidgetClass =
        Settings->TutorialWidgetClass.LoadSynchronous();

    if (!WidgetClass)
    {
        return;
    }

    TutorialWidget =
        CreateWidget<UTutorialWidget>(
            PC,
            WidgetClass
        );

    if (!TutorialWidget)
    {
        return;
    }

    AGameModeBase* GameMode =
        World->GetAuthGameMode();

    if (!GameMode)
    {
        return;
    }

    if (Cast<AIntroGameMode>(GameMode))
    {
        HideMainMenu();
    }
    else if (Cast<AMyPj26GameMode>(GameMode))
    {
        HidePause();
    }

    TutorialWidget->AddToViewport(100);

    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(
        TutorialWidget->TakeWidget()
    );

    PC->SetInputMode(InputMode);
    PC->bShowMouseCursor = true;

    TutorialWidget->FocusButton();
}

// 一時停止画面を生成して表示し、フォーカスと画面操作入力を設定する。
void UGameFlowSubsystem::ShowPause()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    APlayerController* PC =
        World->GetFirstPlayerController();

    if (!PC)
    {
        return;
    }

    if (!IsValid(PauseWidget))
    {
        const UGameFlowSettings* Settings =
            GetDefault<UGameFlowSettings>();

        if (!Settings)
        {
            return;
        }

        TSubclassOf<UPauseWidget> WidgetClass =
            Settings->PauseWidgetClass.LoadSynchronous();

        if (!WidgetClass)
        {
            return;
        }

        PauseWidget =
            CreateWidget<UPauseWidget>(
                PC,
                WidgetClass
            );

        if (!PauseWidget)
        {
            return;
        }

        PauseWidget->AddToViewport();
    }

    PauseWidget->SetVisibility(
        ESlateVisibility::Visible
    );


    UGameplayStatics::SetGamePaused(
        World,
        true
    );


    FInputModeUIOnly InputMode;

    if (UWidget* FocusWidget =
        PauseWidget->GetDefaultFocusWidget())
    {
        InputMode.SetWidgetToFocus(
            FocusWidget->TakeWidget()
        );
    }

    PC->SetInputMode(InputMode);
    PC->bShowMouseCursor = true;


    PauseWidget->FocusButton();
}
// 一時停止画面を閉じ、ゲーム進行とプレイヤー入力を再開する。
void UGameFlowSubsystem::HidePause()
{
    if (PauseWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("PauseWidget->SetVisibility(ESlateVisibility::Collapsed);"));
        PauseWidget->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (UWorld* World = GetWorld())
    {
        if (APlayerController* PC =
            World->GetFirstPlayerController())
        {
            PC->SetShowMouseCursor(false);

            FInputModeGameOnly InputMode;
            PC->SetInputMode(InputMode);
        }
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    UGameplayStatics::SetGamePaused(World, false);
}

// チュートリアル画面を閉じ、保持参照と入力状態を通常へ戻す。
void UGameFlowSubsystem::HideTutorial()
{
    if (TutorialWidget)
    {
        TutorialWidget->RemoveFromParent();
        TutorialWidget = nullptr;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    APlayerController* PlayerController =
        World->GetFirstPlayerController();

    if (!PlayerController)
    {
        return;
    }

    AGameModeBase* GameMode =
        World->GetAuthGameMode();

    if (!GameMode)
    {
        return;
    }

    if (Cast<AIntroGameMode>(GameMode))
    {
        UGameplayStatics::SetGamePaused(World, false);
        ShowMainMenu();
    }
    else if (Cast<AMyPj26GameMode>(GameMode))
    {
        ShowPause();
    }
}

// ボス撃破画面を生成して表示し、次の操作に必要なフォーカスを設定する。
void UGameFlowSubsystem::ShowBossCleared()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    APlayerController* PC =
        World->GetFirstPlayerController();

    if (!PC || BossClearedWidget)
    {
        return;
    }

    const UGameFlowSettings* Settings =
        GetDefault<UGameFlowSettings>();

    if (!Settings)
    {
        return;
    }

    TSubclassOf<UBossClearedWidget> WidgetClass =
        Settings->BossClearedWidgetClass.LoadSynchronous();

    if (!WidgetClass)
    {
        return;
    }

    BossClearedWidget =
        CreateWidget<UBossClearedWidget>(
            PC,
            WidgetClass
        );

    if (!BossClearedWidget)
    {
        return;
    }

    BossClearedWidget->AddToViewport(100);

    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(
        BossClearedWidget->TakeWidget()
    );

    PC->SetInputMode(InputMode);
    PC->bShowMouseCursor = true;

    BossClearedWidget->FocusButton();
}

// ボス撃破画面を閉じ、保持参照と入力状態を通常へ戻す。
void UGameFlowSubsystem::HideBossCleared()
{
    if (BossClearedWidget)
    {
        BossClearedWidget->RemoveFromParent();
        BossClearedWidget = nullptr;
    }

    if (UWorld* World = GetWorld())
    {
        if (APlayerController* PC =
            World->GetFirstPlayerController())
        {
            PC->SetShowMouseCursor(false);

            FInputModeGameOnly InputMode;
            PC->SetInputMode(InputMode);
        }
    }
}

// 生成済みの進行画面をすべて閉じ、保持している画面参照を初期化する。
void UGameFlowSubsystem::ResetWidgets()
{
    if (MainMenuWidget)
    {
        MainMenuWidget->RemoveFromParent();
        MainMenuWidget = nullptr;
    }

    if (PauseWidget)
    {
        PauseWidget->RemoveFromParent();
        PauseWidget = nullptr;
    }

    if (TutorialWidget)
    {
        TutorialWidget->RemoveFromParent();
        TutorialWidget = nullptr;
    }

    if (GameOverWidget)
    {
        GameOverWidget->RemoveFromParent();
        GameOverWidget = nullptr;
    }

    if (BossClearedWidget)
    {
        BossClearedWidget->RemoveFromParent();
        BossClearedWidget = nullptr;
    }
}

// 一時停止画面が現在表示されているか判定して返す。
bool UGameFlowSubsystem::GetIsPauseWidgetOn()
{
    bool result = false;

    if (PauseWidget)
    {
        result = PauseWidget->GetVisibility() == ESlateVisibility::Visible;
    }

    return result;
}
