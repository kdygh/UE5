// ゲーム開始と進行を管理する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "IntroGameMode.generated.h"


class UMainMenuWidget;

// ゲーム開始と進行を管理する役割を持つ。
UCLASS()
class PJ26_API AIntroGameMode : public AGameMode
{
	GENERATED_BODY()

public:
    // 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
    AIntroGameMode();

    TObjectPtr<UMainMenuWidget> GetMainMenuWidget()
    {
        return MainMenuWidget;
    }

protected:
    // ゲーム開始時に必要な参照・イベント・初期状態を設定する。
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

private:
    UPROPERTY()
    TObjectPtr<UMainMenuWidget> MainMenuWidget;

};
