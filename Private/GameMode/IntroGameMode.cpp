// ゲーム開始と進行を管理する。

#include "GameMode/IntroGameMode.h"
#include "UI/MainMenuWidget.h"
#include "GameFramework/PlayerController.h"
#include "Subsystem/GameFlowSettings.h"
#include "Subsystem/GameFlowSubsystem.h"
#include "Subsystem/GameAudioSubsystem.h"
#include "Components/AudioComponent.h"
#include "Data/GameAudioSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
AIntroGameMode::AIntroGameMode()
{

    DefaultPawnClass = nullptr;
}

// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void AIntroGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (UGameAudioSubsystem* AudioSubsystem =
        GetGameInstance()->GetSubsystem<UGameAudioSubsystem>())
    {
        AudioSubsystem->PlayMainMenuBGM();
    }

    if (UGameFlowSubsystem* GameFlowSubsystem =
        GetGameInstance()->GetSubsystem<UGameFlowSubsystem>())
    {
        GameFlowSubsystem->ShowMainMenu();
    }
}
