// アニメーションとゲーム処理を連携する。

#include "MyAnimNotifyStatePJ26.h"
#include "MyPlayer.h"
#include "Engine/GameEngine.h"

// アニメーション通知の開始時に、所有者へ開始処理を伝える。
void UMyAnimNotifyStatePJ26::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Notify Begin"));
	if (!MeshComp) return;
	if (AMyPlayer* Character = Cast<AMyPlayer>(MeshComp->GetOwner()))
	{
		if (Character->GetLightAtkCnt() >= 1)
		{
			Character->OnAnimAction(FName("NextComboEnable"));
		}

		if(Character->GetHeavyAtkCnt() >= 6)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("NextHeavyAtkEnable"));
			Character->OnAnimAction(FName("NextHeavyAtkEnable"));
		}

		if (Character->GetIsEvading() == true)
		{
			Character->OnAnimAction(FName("EvadeEnable"));
		}

		if(Character->GetIsParrying() == true)
		{
			Character->OnAnimAction(FName("ParryEnable"));
		}
	}
}

// アニメーション通知の終了時に、所有者へ終了処理を伝える。
void UMyAnimNotifyStatePJ26::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Notify End"));
	if (!MeshComp) return;
	if (AMyPlayer* Character = Cast<AMyPlayer>(MeshComp->GetOwner()))
	{
		Character->GetAtkQueue();
	}
}
