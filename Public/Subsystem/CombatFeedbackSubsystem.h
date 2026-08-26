// ゲーム全体で共有する機能を管理する。

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CombatFeedbackSubsystem.generated.h"


// ゲーム全体で共有する機能を管理する役割を持つ。
UCLASS()
class PJ26_API UCombatFeedbackSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
    // 命中を設定された条件で再生し、再生状態を更新する。
    void PlayHitStop(float Duration);

    void PlayCameraShake(
        TSubclassOf<UCameraShakeBase> ShakeClass,
        float Scale = 1.f
    );

    // 一時的に変更した時間倍率を通常の値へ戻す。
    void ResetTimeDilation();

private:
	bool IsHitStopActive = false;
};
