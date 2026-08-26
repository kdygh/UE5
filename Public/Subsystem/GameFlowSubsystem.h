// ゲーム全体で共有する機能を管理する。

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameFlowSubsystem.generated.h"


// ゲーム全体で共有する機能を管理する役割を持つ。
UENUM(BlueprintType)
enum class EGameFlowState : uint8
{
    Playing,
    Paused,
    PlayerDead,
    BossCleared,
    Restarting
};

class UGameOverWidget;
class UPlayerHpWidget;
class UPauseWidget;
class UBossClearedWidget;
class UTutorialWidget;
class UMainMenuWidget;

// ゲーム全体で共有する機能を管理する役割を持つ。
UCLASS()
class PJ26_API UGameFlowSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
    // ゲーム進行を一時停止し、一時停止画面と操作入力を有効にする。
    void PauseGame();
    // 一時停止画面を閉じ、ゲーム進行とプレイヤー入力を再開する。
    void ResumeGame();

    // 現在の部屋と戦闘状態を初期化し、再挑戦できる状態へ戻す。
    void RestartCurrentRoom();
    // 演出用プールと各画面を初期化し、指定または現在のレベルを読み直す。
    void StartNewRun(FName levelName = "");

    // 終了要求を受け取り、動作中のゲームを安全に終了する。
    void QuitGame();

public:
    // メインメニュー画面を必要に応じて生成し、画面操作用の入力状態を設定する。
    void ShowMainMenu();
    // メインメニュー画面を閉じ、ゲーム操作用の入力状態へ戻す。
    void HideMainMenu();

    // ゲームオーバー画面を読み込んで表示し、入力を画面操作へ切り替える。
    void ShowGameOver();
    // ゲームオーバー画面を閉じ、保持参照と入力状態を通常へ戻す。
    void HideGameOver();

    // チュートリアル画面を生成して表示し、操作対象を画面へ切り替える。
    void ShowTutorial();
    // 一時停止画面を生成して表示し、フォーカスと画面操作入力を設定する。
    void ShowPause();
    // 一時停止画面を閉じ、ゲーム進行とプレイヤー入力を再開する。
    void HidePause();
    // チュートリアル画面を閉じ、保持参照と入力状態を通常へ戻す。
    void HideTutorial();

    // ボス撃破画面を生成して表示し、次の操作に必要なフォーカスを設定する。
    void ShowBossCleared();
    // ボス撃破画面を閉じ、保持参照と入力状態を通常へ戻す。
    void HideBossCleared();

    // 生成済みの進行画面をすべて閉じ、保持している画面参照を初期化する。
    void ResetWidgets();

    // 一時停止画面が現在表示されているか判定して返す。
    bool GetIsPauseWidgetOn();

protected:
    UPROPERTY()
    TObjectPtr<UMainMenuWidget> MainMenuWidget;

    TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

    UPROPERTY()
    TObjectPtr<UGameOverWidget> GameOverWidget;

    TSubclassOf<UGameOverWidget> GameOverWidgetClass;

    UPROPERTY()
    TObjectPtr<UPauseWidget> PauseWidget;

    TSubclassOf<UPauseWidget> PauseWidgetClass;

    UPROPERTY()
    TObjectPtr<UTutorialWidget> TutorialWidget;

    TSubclassOf<UPauseWidget> TutorialWidgetClass;

    UPROPERTY()
    TObjectPtr<UBossClearedWidget> BossClearedWidget;

    TSubclassOf<UBossClearedWidget> BossClearedWidgetClass;

private:
    EGameFlowState CurrentState;
};
