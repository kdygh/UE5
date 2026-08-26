// ゲームで使用する設定データを定義する。

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameSoundSet.generated.h"


// ゲームで使用する設定データを定義する役割を持つ。
UCLASS()
class PJ26_API UGameSoundSet : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> NormalSwing;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> FireSwing;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LightningSwing;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> WindSwing;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> Parring;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> Hit;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> Explosion;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> Dash;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ItemGet;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ButtonSelect;
};
