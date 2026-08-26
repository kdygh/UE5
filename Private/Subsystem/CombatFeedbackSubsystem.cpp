// ゲーム全体で共有する機能を管理する。

#include "Subsystem/CombatFeedbackSubsystem.h"
#include <Kismet/GameplayStatics.h>

// 命中を設定された条件で再生し、再生状態を更新する。
void UCombatFeedbackSubsystem::PlayHitStop(float Duration)
{
    if(IsHitStopActive == true)
    {
        return;
	}

    IsHitStopActive = true;

    UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.007f);

    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle,
        FTimerDelegate::CreateUObject(this, &UCombatFeedbackSubsystem::ResetTimeDilation),
        Duration, false);
}

// カメラを設定された条件で再生し、再生状態を更新する。
void UCombatFeedbackSubsystem::PlayCameraShake(TSubclassOf<UCameraShakeBase> ShakeClass, float Scale)
{
    if (!ShakeClass)
    {
		UE_LOG(LogTemp, Warning, TEXT("PlayCameraShake: ShakeClass is null."));
        return;
    }

    UWorld* World = GetWorld();

    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayCameraShake: GetWorld is null."));

        return;
    }

    APlayerController* PlayerController =
        World->GetFirstPlayerController();

    if (!PlayerController ||
        !PlayerController->PlayerCameraManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayCameraShake: PlayerController is null."));

        return;
    }

    PlayerController->PlayerCameraManager->StartCameraShake(
        ShakeClass,
        Scale
    );
}

// 一時的に変更した時間倍率を通常の値へ戻す。
void UCombatFeedbackSubsystem::ResetTimeDilation()
{
	IsHitStopActive = false;
    UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
}
