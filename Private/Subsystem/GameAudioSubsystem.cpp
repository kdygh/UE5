// ゲーム全体で共有する機能を管理する。

#include "Subsystem/GameAudioSubsystem.h"
#include "Data/GameAudioSettings.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

// 対象の参照と初期状態を設定し、使用可能な状態にする。
void UGameAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    const UGameAudioSettings* Settings =
        GetDefault<UGameAudioSettings>();

    if (!Settings)
    {
        return;
    }

    GameSoundSet =
        Settings->GameSoundSet.LoadSynchronous();
}

// 武器効果音を設定された条件で再生し、再生状態を更新する。
void UGameAudioSubsystem::PlayWeaponSwingSound(const FVector& Location)
{
    if (!GameSoundSet ||
        !GameSoundSet->NormalSwing)
    {
        return;
    }

    UWorld* World = GetWorld();

    if (!World)
    {
        return;
    }

    UGameplayStatics::PlaySoundAtLocation(
        World,
        GameSoundSet->NormalSwing,
        Location
    );
}

// 炎武器効果音を設定された条件で再生し、再生状態を更新する。
void UGameAudioSubsystem::PlayFireWeaponSwingSound(const FVector& Location)
{
    if (!GameSoundSet ||
        !GameSoundSet->FireSwing)
    {
        return;
    }

    UWorld* World = GetWorld();

    if (!World)
    {
        return;
    }

    UGameplayStatics::PlaySoundAtLocation(
        World,
        GameSoundSet->FireSwing,
        Location
    );
}

// 雷武器効果音を設定された条件で再生し、再生状態を更新する。
void UGameAudioSubsystem::PlayLightningWeaponSwingSound(const FVector& Location)
{
    if (!GameSoundSet ||
        !GameSoundSet->LightningSwing)
    {
        return;
    }

    UWorld* World = GetWorld();

    if (!World)
    {
        return;
    }

    UGameplayStatics::PlaySoundAtLocation(
        World,
        GameSoundSet->LightningSwing,
        Location
    );
}

// 風武器効果音を設定された条件で再生し、再生状態を更新する。
void UGameAudioSubsystem::PlayWindWeaponSwingSound(const FVector& Location)
{
    if (!GameSoundSet ||
        !GameSoundSet->WindSwing)
    {
        return;
    }

    UWorld* World = GetWorld();

    if (!World)
    {
        return;
    }

    UGameplayStatics::PlaySoundAtLocation(
        World,
        GameSoundSet->WindSwing,
        Location
    );
}

// パリィ状態効果音を設定された条件で再生し、再生状態を更新する。
void UGameAudioSubsystem::PlayParryingSound(const FVector& Location)
{
    if (!GameSoundSet ||
        !GameSoundSet->Parring)
    {
        return;
    }

    UWorld* World = GetWorld();

    if (!World)
    {
        return;
    }

    UGameplayStatics::PlaySoundAtLocation(
        World,
        GameSoundSet->Parring,
        Location
    );
}

// 命中効果音を設定された条件で再生し、再生状態を更新する。
void UGameAudioSubsystem::PlayHitSound(const FVector& Location)
{
    if (!GameSoundSet ||
        !GameSoundSet->Hit)
    {
        return;
    }

    UWorld* World = GetWorld();

    if (!World)
    {
        return;
    }

    UGameplayStatics::PlaySoundAtLocation(
        World,
        GameSoundSet->Hit,
        Location
    );
}

// 爆発効果音を設定された条件で再生し、再生状態を更新する。
void UGameAudioSubsystem::PlayExplosionSound(const FVector& Location)
{
    if (!GameSoundSet ||
        !GameSoundSet->Explosion)
    {
        return;
    }

    UWorld* World = GetWorld();

    if (!World)
    {
        return;
    }

    UGameplayStatics::PlaySoundAtLocation(
        World,
        GameSoundSet->Explosion,
        Location
    );
}

// 突進効果音を設定された条件で再生し、再生状態を更新する。
void UGameAudioSubsystem::PlayDashSound(const FVector& Location)
{
    if (!GameSoundSet ||
        !GameSoundSet->Dash)
    {
        return;
    }

    UWorld* World = GetWorld();

    if (!World)
    {
        return;
    }

    UGameplayStatics::PlaySoundAtLocation(
        World,
        GameSoundSet->Dash,
        Location
    );
}

// アイテム効果音を設定された条件で再生し、再生状態を更新する。
void UGameAudioSubsystem::PlayItemGetSound(const FVector& Location)
{
    if (!GameSoundSet ||
        !GameSoundSet->ItemGet)
    {
        return;
    }

    UWorld* World = GetWorld();

    if (!World)
    {
        return;
    }

    UGameplayStatics::PlaySoundAtLocation(
        World,
        GameSoundSet->ItemGet,
        Location
    );
}

// 効果音を設定された条件で再生し、再生状態を更新する。
void UGameAudioSubsystem::PlayButtonSelectSound()
{
    if (!GameSoundSet ||
        !GameSoundSet->ButtonSelect)
    {
        return;
    }

    UWorld* World = GetWorld();

    if (!World)
    {
        return;
    }

    UGameplayStatics::PlaySound2D(
        World,
        GameSoundSet->ButtonSelect);
}

// メニューを設定された条件で再生し、再生状態を更新する。
void UGameAudioSubsystem::PlayMainMenuBGM()
{
    const UGameAudioSettings* Settings =
        GetDefault<UGameAudioSettings>();

    if (!Settings)
    {
        return;
    }

    USoundBase* Sound =
        Settings->MainMenuBGM.LoadSynchronous();

    if (!Sound)
    {
        return;
    }

    if (CurrentBGMComponent)
    {
        CurrentBGMComponent->Stop();
        CurrentBGMComponent = nullptr;
    }

    CurrentBGMComponent =
        UGameplayStatics::SpawnSound2D(
            GetGameInstance(),
            Sound,
            1.f,
            1.f,
            0.f,
            nullptr,
            true,
            false
        );
}

// ステージを設定された条件で再生し、再生状態を更新する。
void UGameAudioSubsystem::PlayStageBGM()
{
    const UGameAudioSettings* Settings =
        GetDefault<UGameAudioSettings>();

    if (!Settings)
    {
        return;
    }

    USoundBase* Sound =
        Settings->StageBGM.LoadSynchronous();

    if (!Sound)
    {
        return;
    }

    if (CurrentBGMComponent)
    {
        CurrentBGMComponent->Stop();
        CurrentBGMComponent = nullptr;
    }

    CurrentBGMComponent =
        UGameplayStatics::SpawnSound2D(
            GetGameInstance(),
            Sound,
            1.f,
            1.f,
            0.f,
            nullptr,
            true,
            false
        );
}

// ボスを設定された条件で再生し、再生状態を更新する。
void UGameAudioSubsystem::PlayBossBGM()
{
    const UGameAudioSettings* Settings =
        GetDefault<UGameAudioSettings>();

    if (!Settings)
    {
        return;
    }

    USoundBase* Sound =
        Settings->BossBGM.LoadSynchronous();

    if (!Sound)
    {
        return;
    }

    StopBGM();

    CurrentBGMComponent =
        UGameplayStatics::SpawnSound2D(
            GetGameInstance(),
            Sound,
            1.f,
            1.f,
            0.f,
            nullptr,
            true,
            true
        );
}

// 対象を終了し、関連状態とタイマーを解除する。
void UGameAudioSubsystem::StopBGM(float FadeOutTime)
{
    if (!IsValid(CurrentBGMComponent))
    {
        CurrentBGMComponent = nullptr;
        return;
    }

    if (FadeOutTime > 0.f)
    {
        CurrentBGMComponent->FadeOut(
            FadeOutTime,
            0.f
        );
    }
    else
    {
        CurrentBGMComponent->Stop();
    }

    CurrentBGMComponent = nullptr;
}
