// ゲーム全体で共有する機能を管理する。

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "EffectSettings.generated.h"


class UNiagaraSystem;

// ゲーム全体で共有する機能を管理する役割を持つ。
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Effect Settings"))
class PJ26_API UEffectSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	virtual FName GetCategoryName() const override
	{
		return TEXT("Game");
	}

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Common FX")
	TSoftObjectPtr<UNiagaraSystem> HitFX;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Common FX")
	TSoftObjectPtr<UNiagaraSystem> ExplosionFX;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Common FX")
	TSoftObjectPtr<UNiagaraSystem> ParryFX;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Common FX")
	TSoftObjectPtr<UNiagaraSystem> ShockwaveFX;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Common FX")
	TSoftObjectPtr<UNiagaraSystem> HitLightningFX1;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Common FX")
	TSoftObjectPtr<UNiagaraSystem> HitLightningFX2;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Common FX")
	TSoftObjectPtr<UNiagaraSystem> HitLightningFX3;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Common FX")
	TSoftObjectPtr<UNiagaraSystem> HitWindFX3;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Common FX")
	TSoftObjectPtr<UNiagaraSystem> RecoverHPFX;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Common FX")
	TSoftObjectPtr<UNiagaraSystem> IncreaseMaxHPFX;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Common FX")
	TSoftObjectPtr<UNiagaraSystem> IncreaseMaxSPFX;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Common FX")
	TSoftObjectPtr<UNiagaraSystem> IncreaseAtkPowerFX;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Common FX")
	TSoftObjectPtr<UNiagaraSystem> IncreaseDefFX;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Common FX")
	TSoftObjectPtr<UNiagaraSystem> IncreaseSpeedFX;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Common FX")
	TSoftObjectPtr<UNiagaraSystem> DistortionFX;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Common FX")
	TSoftObjectPtr<UNiagaraSystem> FireStatusFX;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Common FX")
	TSoftObjectPtr<UNiagaraSystem> LightningStatusFX;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Common FX")
	TSoftObjectPtr<UNiagaraSystem> WindStatusFX;
};
