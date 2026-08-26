// ゲームで使用する設定データを定義する。

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Data/GameSoundSet.h"
#include "GameAudioSettings.generated.h"


// ゲームで使用する設定データを定義する役割を持つ。
UCLASS(Config = Game, DefaultConfig)
class PJ26_API UGameAudioSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
    UPROPERTY(Config, EditAnywhere, Category = "Music")
    TSoftObjectPtr<USoundBase> MainMenuBGM;

    UPROPERTY(Config, EditAnywhere, Category = "Music")
    TSoftObjectPtr<USoundBase> StageBGM;

    UPROPERTY(Config, EditAnywhere, Category = "Music")
    TSoftObjectPtr<USoundBase> BossBGM;

    UPROPERTY(Config, EditAnywhere, Category = "UI")
    TSoftObjectPtr<USoundBase> ButtonSelectSound;

public:
    UPROPERTY(Config, EditAnywhere, Category = "Combat")
    TSoftObjectPtr<UGameSoundSet> GameSoundSet;
};
