// ゲーム全体で共有する機能を管理する。

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameFlowSettings.generated.h"


class UGameOverWidget;
class UPauseWidget;
class UBossClearedWidget;
class UMainMenuWidget;
class UTutorialWidget;

// ゲーム全体で共有する機能を管理する役割を持つ。
UCLASS(Config = Game, DefaultConfig)
class PJ26_API UGameFlowSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
    UPROPERTY(
        Config,
        EditAnywhere,
        Category = "UI"
    )
    TSoftClassPtr<UGameOverWidget> GameOverWidgetClass;

    UPROPERTY(
        Config,
        EditAnywhere,
        Category = "UI"
    )
    TSoftClassPtr<UPauseWidget> PauseWidgetClass;

    UPROPERTY(
        Config,
        EditAnywhere,
        Category = "UI"
    )
    TSoftClassPtr<UBossClearedWidget> BossClearedWidgetClass;

    UPROPERTY(
        Config,
        EditAnywhere,
        Category = "UI"
    )
    TSoftClassPtr<UMainMenuWidget> MainMenuWidgetClass;

    UPROPERTY(
        Config,
        EditAnywhere,
        Category = "UI"
    )
    TSoftClassPtr<UTutorialWidget> TutorialWidgetClass;
};
