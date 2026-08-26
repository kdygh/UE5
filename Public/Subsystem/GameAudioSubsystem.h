// ゲーム全体で共有する機能を管理する。

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameAudioSubsystem.generated.h"


class UGameSoundSet;
class USoundBase;
class UAudioComponent;

// ゲーム全体で共有する機能を管理する役割を持つ。
UCLASS()
class PJ26_API UGameAudioSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
    // 対象の参照と初期状態を設定し、使用可能な状態にする。
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
    UGameSoundSet* GetPlayerCombatSoundSet() const
    {
        return GameSoundSet;
    }

public:
    // 武器効果音を設定された条件で再生し、再生状態を更新する。
    void PlayWeaponSwingSound(const FVector& Location);
    // 炎武器効果音を設定された条件で再生し、再生状態を更新する。
    void PlayFireWeaponSwingSound(const FVector& Location);
    // 雷武器効果音を設定された条件で再生し、再生状態を更新する。
    void PlayLightningWeaponSwingSound(const FVector& Location);
    // 風武器効果音を設定された条件で再生し、再生状態を更新する。
    void PlayWindWeaponSwingSound(const FVector& Location);
    // パリィ状態効果音を設定された条件で再生し、再生状態を更新する。
    void PlayParryingSound(const FVector& Location);
    // 命中効果音を設定された条件で再生し、再生状態を更新する。
    void PlayHitSound(const FVector& Location);
    // 爆発効果音を設定された条件で再生し、再生状態を更新する。
    void PlayExplosionSound(const FVector& Location);
    // 突進効果音を設定された条件で再生し、再生状態を更新する。
    void PlayDashSound(const FVector& Location);
    // アイテム効果音を設定された条件で再生し、再生状態を更新する。
    void PlayItemGetSound(const FVector& Location);
    // 効果音を設定された条件で再生し、再生状態を更新する。
    void PlayButtonSelectSound();

public:
    // メニューを設定された条件で再生し、再生状態を更新する。
    void PlayMainMenuBGM();
    // ステージを設定された条件で再生し、再生状態を更新する。
    void PlayStageBGM();
    // ボスを設定された条件で再生し、再生状態を更新する。
    void PlayBossBGM();

    // 対象を終了し、関連状態とタイマーを解除する。
    void StopBGM(float FadeOutTime = 1.f);

protected:
    UPROPERTY()
    TObjectPtr<UGameSoundSet> GameSoundSet;

    UPROPERTY()
    TObjectPtr<UAudioComponent> CurrentBGMComponent;
};
