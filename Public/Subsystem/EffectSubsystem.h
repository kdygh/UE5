// ゲーム全体で共有する機能を管理する。

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "NiagaraCommon.h"
#include "EffectSubsystem.generated.h"


class UNiagaraSystem;
class UNiagaraComponent;
class USceneComponent;

// ゲーム全体で共有する機能を管理する役割を持つ。
UCLASS()
class PJ26_API UEffectSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

protected:
	// 対象の参照と初期状態を設定し、使用可能な状態にする。
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	// 共通して使用するナイアガラ資産を非同期で読み込み、再利用用の参照を保存する。
	void PreloadCommonFX();

public:
	UFUNCTION(BlueprintCallable, Category = "FX")
	UNiagaraComponent* SpawnFXAtLocation(
		UNiagaraSystem* NiagaraSystem,
		const FVector& Location,
		const FRotator& Rotation = FRotator::ZeroRotator,
		const FVector& Scale = FVector(1.f),
		ENCPoolMethod PoolingMethod = ENCPoolMethod::AutoRelease
	);

	// 管理中のナイアガラ構成要素を停止・破棄し、有効効果の一覧を空にする。
	void DeactiveAllEffects();

public:
	UFUNCTION(BlueprintCallable, Category = "FX")
	UNiagaraComponent* SpawnHitFX(
		const FVector& Location,
		const FRotator& Rotation = FRotator::ZeroRotator
	);

	UFUNCTION(BlueprintCallable, Category = "FX")
	UNiagaraComponent* SpawnExplosionFX(
		const FVector& Location,
		const FRotator& Rotation = FRotator::ZeroRotator
	);

	UFUNCTION(BlueprintCallable, Category = "FX")
	UNiagaraComponent* SpawnParryFX(
		const FVector& Location,
		const FRotator& Rotation = FRotator::ZeroRotator
	);

	UFUNCTION(BlueprintCallable, Category = "FX")
	UNiagaraComponent* SpawnShockwaveFX(
		const FVector& Location,
		const FRotator& Rotation = FRotator::ZeroRotator
	);

	UFUNCTION(BlueprintCallable, Category = "FX")
	UNiagaraComponent* SpawnLightningFX(
		const FVector& Location,
		const FRotator& Rotation = FRotator::ZeroRotator
	);

	UFUNCTION(BlueprintCallable, Category = "FX")
	UNiagaraComponent* SpawnWindFX(
		const FVector& Location,
		const FRotator& Rotation = FRotator::ZeroRotator
	);

	UFUNCTION(BlueprintCallable, Category = "FX")
	UNiagaraComponent* SpawnRecoverHPFX(
		const FVector& Location,
		const FRotator& Rotation = FRotator::ZeroRotator
	);

	UFUNCTION(BlueprintCallable, Category = "FX")
	UNiagaraComponent* SpawnIncreaseMaxHPFX(
		const FVector& Location,
		const FRotator& Rotation = FRotator::ZeroRotator
	);

	UFUNCTION(BlueprintCallable, Category = "FX")
	UNiagaraComponent* SpawnIncreaseMaxSPFX(
		const FVector& Location,
		const FRotator& Rotation = FRotator::ZeroRotator
	);

	UFUNCTION(BlueprintCallable, Category = "FX")
	UNiagaraComponent* SpawnIncreaseDefFX(
		const FVector& Location,
		const FRotator& Rotation = FRotator::ZeroRotator
	);

	UFUNCTION(BlueprintCallable, Category = "FX")
	UNiagaraComponent* SpawnIncreaseSpeedFX(
		const FVector& Location,
		const FRotator& Rotation = FRotator::ZeroRotator
	);

	UFUNCTION(BlueprintCallable, Category = "FX")
	UNiagaraComponent* SpawnIncreaseAtkPowerFX(
		const FVector& Location,
		const FRotator& Rotation = FRotator::ZeroRotator
	);

	UFUNCTION(BlueprintCallable, Category = "FX")
	UNiagaraComponent* SpawnDistortionShockFX(
		const FVector& Location,
		const FRotator& Rotation = FRotator::ZeroRotator
	);

	UFUNCTION(BlueprintCallable, Category = "FX")
	UNiagaraComponent* SpawnFireStatusFX(
		const FVector& Location,
		const FRotator& Rotation = FRotator::ZeroRotator
	);

	UFUNCTION(BlueprintCallable, Category = "FX")
	UNiagaraComponent* SpawnLightningStatusFX(
		const FVector& Location,
		const FRotator& Rotation = FRotator::ZeroRotator
	);

	UFUNCTION(BlueprintCallable, Category = "FX")
	UNiagaraComponent* SpawnWindStatusFX(
		const FVector& Location,
		const FRotator& Rotation = FRotator::ZeroRotator
	);

private:
	UPROPERTY()
	TObjectPtr<UNiagaraSystem> LoadedHitFX = nullptr;

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> LoadedExplosionFX = nullptr;

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> LoadedParryFX = nullptr;

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> LoadedShockwaveFX = nullptr;

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> LoadedLightningFX = nullptr;

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> LoadedWindFX = nullptr;

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> LoadedRecoverHPFX = nullptr;

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> LoadedIncreaseMaxHPFX = nullptr;

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> LoadedIncreaseMaxSPFX = nullptr;

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> LoadedIncreaseDefFX = nullptr;

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> LoadedIncreaseSpeedFX = nullptr;

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> LoadedIncreaseAtkPowerFX = nullptr;

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> LoadedDistortionShockFX = nullptr;

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> LoadedFireStatusFX = nullptr;

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> LoadedLightningStatusFX = nullptr;

	UPROPERTY()
	TObjectPtr<UNiagaraSystem> LoadedWindStatusFX = nullptr;

private:
	UPROPERTY()
	TArray<TWeakObjectPtr<UNiagaraComponent>> ActiveEffects;
};
