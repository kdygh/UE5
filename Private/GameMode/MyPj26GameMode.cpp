// ゲーム開始と進行を管理する。

#include "GameMode/MyPj26GameMode.h"
#include "Subsystem/EffectActorSubsystem.h"
#include "Subsystem/RoomSubsystem.h"
#include "Subsystem/GameAudioSubsystem.h"
#include "Objects/LightningActor.h"
#include "Objects/ShockwaveActor.h"
#include "../../Room.h"
#include "Components/AudioComponent.h"
#include "Data/GameAudioSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "UI/LoadingWidget.h"

// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void AMyPj26GameMode::BeginPlay()
{
	Super::BeginPlay();

    ShowLoadingWidget();

	if(UEffectActorSubsystem* EffectActorSubsystem = GetWorld()->GetSubsystem<UEffectActorSubsystem>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Prewarming lightning pool"));
		EffectActorSubsystem->PrewarmPool(AShockwaveActor::StaticClass(), 10);
	}

	LoadStage();
}

// 構成要素の生成完了後に、相互参照とイベントを接続する。
void AMyPj26GameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

// 読み込むステージとロード画面を設定した後、レベル遷移を開始する。
void AMyPj26GameMode::LoadStage()
{
    const FName LevelName(TEXT("Level1"));

    ULevelStreaming* StreamingLevel =
        UGameplayStatics::GetStreamingLevel(
            this,
            LevelName
        );

    if (!StreamingLevel)
    {
        UE_LOG(LogTemp, Error,
            TEXT("Streaming Level not found: %s"),
            *LevelName.ToString());

        return;
    }


    if (StreamingLevel->IsLevelVisible())
    {
        UE_LOG(LogTemp, Warning,
            TEXT("Level1 is already visible"));

        OnLevel1Shown();
        return;
    }

    StreamingLevel->OnLevelShown.AddDynamic(
        this,
        &AMyPj26GameMode::OnLevel1Shown
    );

    FLatentActionInfo LatentInfo;

    UGameplayStatics::LoadStreamLevel(
        this,
        LevelName,
        true,
        false,
        LatentInfo
    );
}

// 対象レベルの表示完了後、ロード画面を閉じてゲーム進行を再開する。
void AMyPj26GameMode::OnLevel1Shown()
{
    if (LoadingWidget)
    {
        LoadingWidget->PlayFadeOut();
    }

    if (UGameAudioSubsystem* AudioSubsystem =
        GetGameInstance()->GetSubsystem<UGameAudioSubsystem>())
    {
        AudioSubsystem->PlayStageBGM();
    }

	StartNewGame();
}

// 新しいゲームに必要な状態を初期化し、最初のステージへ移動する。
void AMyPj26GameMode::StartNewGame()
{
	if (URoomSubsystem* RoomSubsystem = GetWorld()->GetSubsystem<URoomSubsystem>())
	{
		UE_LOG(LogTemp, Warning, TEXT("RoomTemplate injection"));

		if(CombatRoomTemplate.Num() > 0)
		{
			RoomSubsystem->SetCombatRoomClass(CombatRoomTemplate);
		}
		if(ItemRoomTemplate.Num() > 0)
		{
			RoomSubsystem->SetItemRoomClass(ItemRoomTemplate);
		}
		if(BossRoomTemplate.Num() > 0)
		{
			RoomSubsystem->SetBossRoomClass(BossRoomTemplate);
		}

		RoomSubsystem->AddSpawnedRoomLocation(FIntPoint(0, 0));
		RoomSubsystem->GenerateGridLocation(FIntPoint(0, 0), 1);
	}
}

// ロード画面部品を生成して表示し、入力状態を調整する。
void AMyPj26GameMode::ShowLoadingWidget()
{
    APlayerController* PC =
        GetWorld()->GetFirstPlayerController();

    if (!PC || !LoadingWidgetClass)
    {
        return;
    }

    LoadingWidget =
        CreateWidget<ULoadingWidget>(
            PC,
            LoadingWidgetClass
        );

    if (LoadingWidget)
    {
        LoadingWidget->AddToViewport(1000);
    }
}
