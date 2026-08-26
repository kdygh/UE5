// ゲーム全体で共有する機能を管理する。

#include "Subsystem/EffectSubsystem.h"
#include "Subsystem/EffectSettings.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Engine/World.h"
#include "UObject/UObjectGlobals.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

// 対象の参照と初期状態を設定し、使用可能な状態にする。
void UEffectSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	PreloadCommonFX();
}

// 共通して使用するナイアガラ資産を非同期で読み込み、再利用用の参照を保存する。
void UEffectSubsystem::PreloadCommonFX()
{
	const UEffectSettings* FXSettings = GetDefault<UEffectSettings>();
	if (!FXSettings)
	{
		return;
	}

	TArray<FSoftObjectPath> AssetsToLoad;
	if (!FXSettings->HitFX.IsNull()) AssetsToLoad.Add(FXSettings->HitFX.ToSoftObjectPath());
	if (!FXSettings->ExplosionFX.IsNull()) AssetsToLoad.Add(FXSettings->ExplosionFX.ToSoftObjectPath());
	if (!FXSettings->ParryFX.IsNull()) AssetsToLoad.Add(FXSettings->ParryFX.ToSoftObjectPath());
	if (!FXSettings->ShockwaveFX.IsNull()) AssetsToLoad.Add(FXSettings->ShockwaveFX.ToSoftObjectPath());
	if (!FXSettings->HitLightningFX3.IsNull()) AssetsToLoad.Add(FXSettings->HitLightningFX3.ToSoftObjectPath());
	if (!FXSettings->HitWindFX3.IsNull()) AssetsToLoad.Add(FXSettings->HitWindFX3.ToSoftObjectPath());
	if (!FXSettings->DistortionFX.IsNull()) AssetsToLoad.Add(FXSettings->DistortionFX.ToSoftObjectPath());

	if (AssetsToLoad.IsEmpty())
	{
		return;
	}

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(
		AssetsToLoad,
		FStreamableDelegate::CreateUObject(this, &UEffectSubsystem::PreloadCommonFX)
	);


	LoadedHitFX = FXSettings->HitFX.Get();
	LoadedExplosionFX = FXSettings->ExplosionFX.Get();
	LoadedParryFX = FXSettings->ParryFX.Get();
	LoadedShockwaveFX = FXSettings->ShockwaveFX.Get();
	LoadedLightningFX = FXSettings->HitLightningFX3.Get();
	LoadedWindFX = FXSettings->HitWindFX3.Get();
	LoadedDistortionShockFX = FXSettings->DistortionFX.Get();
}

// ワールドと効果の有効性を確認し、指定位置へプール方式でナイアガラ効果を生成する。
UNiagaraComponent* UEffectSubsystem::SpawnFXAtLocation(UNiagaraSystem* NiagaraSystem,
	const FVector& Location,
	const FRotator& Rotation,
	const FVector& Scale,
	ENCPoolMethod PoolingMethod)
{
	if (!GetWorld())
	{
		UE_LOG(LogTemp, Warning, TEXT("UFXSubsystem::SpawnFXAtLocation - World is nullptr"));
		return nullptr;
	}

	if (!NiagaraSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("UFXSubsystem::SpawnFXAtLocation - NiagaraSystem is nullptr"));
		return nullptr;
	}

	return UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		NiagaraSystem,
		Location,
		Rotation,
		Scale,
		true,
		true,
		PoolingMethod,
		true
	);
}

// 管理中のナイアガラ構成要素を停止・破棄し、有効効果の一覧を空にする。
void UEffectSubsystem::DeactiveAllEffects()
{
	for (TWeakObjectPtr<UNiagaraComponent>& WeakComp : ActiveEffects)
	{
		UNiagaraComponent* Comp = WeakComp.Get();

		if (!IsValid(Comp))
		{
			continue;
		}

		Comp->SetRenderingEnabled(false);
		Comp->DeactivateImmediate();
		Comp->DestroyInstance();
		Comp->DestroyComponent();
	}

	ActiveEffects.Empty();
}

// 命中効果を必要に応じて読み込み、指定位置へプール方式で生成する。
UNiagaraComponent* UEffectSubsystem::SpawnHitFX(const FVector& Location, const FRotator& Rotation)
{
	if (!LoadedHitFX)
	{
		const UEffectSettings* FXSettings =
			GetDefault<UEffectSettings>();

		if (!FXSettings)
		{
			return nullptr;
		}

		LoadedHitFX =
			FXSettings->HitFX.LoadSynchronous();
	}

	if (!LoadedHitFX)
	{
		return nullptr;
	}

	UNiagaraComponent* NiagaraComp =
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			LoadedHitFX,
			Location,
			Rotation,
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::AutoRelease,
			true
		);

	if (IsValid(NiagaraComp))
	{
		ActiveEffects.AddUnique(
			TWeakObjectPtr<UNiagaraComponent>(NiagaraComp)
		);
	}

	return NiagaraComp;
}

// 爆発効果を必要に応じて読み込み、指定位置へプール方式で生成する。
UNiagaraComponent* UEffectSubsystem::SpawnExplosionFX(const FVector& Location, const FRotator& Rotation)
{
	if (!LoadedExplosionFX)
	{
		const UEffectSettings* FXSettings =
			GetDefault<UEffectSettings>();

		if (!FXSettings)
		{
			return nullptr;
		}

		LoadedExplosionFX =
			FXSettings->ExplosionFX.LoadSynchronous();
	}

	if (!LoadedExplosionFX)
	{
		return nullptr;
	}

	UNiagaraComponent* NiagaraComp =
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			LoadedExplosionFX,
			Location,
			Rotation,
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::AutoRelease,
			true
		);

	if (IsValid(NiagaraComp))
	{
		ActiveEffects.AddUnique(
			TWeakObjectPtr<UNiagaraComponent>(NiagaraComp)
		);
	}

	return NiagaraComp;
}

// パリィ効果を必要に応じて読み込み、指定位置へプール方式で生成する。
UNiagaraComponent* UEffectSubsystem::SpawnParryFX(const FVector& Location, const FRotator& Rotation)
{
	if (!LoadedParryFX)
	{
		const UEffectSettings* FXSettings =
			GetDefault<UEffectSettings>();

		if (!FXSettings)
		{
			return nullptr;
		}

		LoadedParryFX =
			FXSettings->ParryFX.LoadSynchronous();
	}

	if (!LoadedParryFX)
	{
		return nullptr;
	}

	UNiagaraComponent* NiagaraComp =
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			LoadedParryFX,
			Location,
			Rotation,
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::AutoRelease,
			true
		);

	if (IsValid(NiagaraComp))
	{
		ActiveEffects.AddUnique(
			TWeakObjectPtr<UNiagaraComponent>(NiagaraComp)
		);
	}

	return NiagaraComp;
}

// 衝撃波効果を必要に応じて読み込み、指定位置へプール方式で生成する。
UNiagaraComponent* UEffectSubsystem::SpawnShockwaveFX(const FVector& Location, const FRotator& Rotation)
{
	if (!LoadedShockwaveFX)
	{
		const UEffectSettings* FXSettings =
			GetDefault<UEffectSettings>();

		if (!FXSettings)
		{
			return nullptr;
		}

		LoadedShockwaveFX =
			FXSettings->ShockwaveFX.LoadSynchronous();
	}

	if (!LoadedShockwaveFX)
	{
		return nullptr;
	}

	UNiagaraComponent* NiagaraComp =
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			LoadedShockwaveFX,
			Location,
			Rotation,
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::AutoRelease,
			true
		);

	if (IsValid(NiagaraComp))
	{
		ActiveEffects.AddUnique(
			TWeakObjectPtr<UNiagaraComponent>(NiagaraComp)
		);
	}

	return NiagaraComp;
}

// 雷効果を必要に応じて読み込み、指定位置へプール方式で生成する。
UNiagaraComponent* UEffectSubsystem::SpawnLightningFX(const FVector& Location, const FRotator& Rotation)
{
	if (!LoadedLightningFX)
	{
		const UEffectSettings* FXSettings =
			GetDefault<UEffectSettings>();

		if (!FXSettings)
		{
			return nullptr;
		}

		LoadedLightningFX =
			FXSettings->HitLightningFX3.LoadSynchronous();
	}

	if (!LoadedLightningFX)
	{
		return nullptr;
	}

	UNiagaraComponent* NiagaraComp =
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			LoadedLightningFX,
			Location,
			Rotation,
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::AutoRelease,
			true
		);

	if (IsValid(NiagaraComp))
	{
		ActiveEffects.AddUnique(
			TWeakObjectPtr<UNiagaraComponent>(NiagaraComp)
		);
	}

	return NiagaraComp;
}

// 風効果を必要に応じて読み込み、指定位置へプール方式で生成する。
UNiagaraComponent* UEffectSubsystem::SpawnWindFX(const FVector& Location, const FRotator& Rotation)
{
	if (!LoadedWindFX)
	{
		const UEffectSettings* FXSettings =
			GetDefault<UEffectSettings>();

		if (!FXSettings)
		{
			return nullptr;
		}

		LoadedWindFX =
			FXSettings->HitWindFX3.LoadSynchronous();
	}

	if (!LoadedWindFX)
	{
		return nullptr;
	}

	UNiagaraComponent* NiagaraComp =
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			LoadedWindFX,
			Location,
			Rotation,
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::AutoRelease,
			true
		);

	if (IsValid(NiagaraComp))
	{
		ActiveEffects.AddUnique(
			TWeakObjectPtr<UNiagaraComponent>(NiagaraComp)
		);
	}

	return NiagaraComp;
}

// 体力回復効果を必要に応じて読み込み、指定位置へプール方式で生成する。
UNiagaraComponent* UEffectSubsystem::SpawnRecoverHPFX(const FVector& Location, const FRotator& Rotation)
{
	if (!LoadedRecoverHPFX)
	{
		const UEffectSettings* FXSettings =
			GetDefault<UEffectSettings>();

		if (!FXSettings)
		{
			return nullptr;
		}

		LoadedRecoverHPFX =
			FXSettings->RecoverHPFX.LoadSynchronous();
	}

	if (!LoadedRecoverHPFX)
	{
		return nullptr;
	}

	UNiagaraComponent* NiagaraComp =
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			LoadedRecoverHPFX,
			Location,
			Rotation,
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::AutoRelease,
			true
		);

	if (IsValid(NiagaraComp))
	{
		ActiveEffects.AddUnique(
			TWeakObjectPtr<UNiagaraComponent>(NiagaraComp)
		);
	}

	return NiagaraComp;
}

// 最大体力増加効果を必要に応じて読み込み、指定位置へプール方式で生成する。
UNiagaraComponent* UEffectSubsystem::SpawnIncreaseMaxHPFX(const FVector& Location, const FRotator& Rotation)
{
	if (!LoadedIncreaseMaxHPFX)
	{
		const UEffectSettings* FXSettings =
			GetDefault<UEffectSettings>();

		if (!FXSettings)
		{
			return nullptr;
		}

		LoadedIncreaseMaxHPFX =
			FXSettings->IncreaseMaxHPFX.LoadSynchronous();
	}

	if (!LoadedIncreaseMaxHPFX)
	{
		return nullptr;
	}

	UNiagaraComponent* NiagaraComp =
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			LoadedIncreaseMaxHPFX,
			Location,
			Rotation,
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::AutoRelease,
			true
		);

	if (IsValid(NiagaraComp))
	{
		ActiveEffects.AddUnique(
			TWeakObjectPtr<UNiagaraComponent>(NiagaraComp)
		);
	}

	return NiagaraComp;
}

// 最大スタミナ増加効果を必要に応じて読み込み、指定位置へプール方式で生成する。
UNiagaraComponent* UEffectSubsystem::SpawnIncreaseMaxSPFX(const FVector& Location, const FRotator& Rotation)
{
	if (!LoadedIncreaseMaxSPFX)
	{
		const UEffectSettings* FXSettings =
			GetDefault<UEffectSettings>();

		if (!FXSettings)
		{
			return nullptr;
		}

		LoadedIncreaseMaxSPFX =
			FXSettings->IncreaseMaxSPFX.LoadSynchronous();
	}

	if (!LoadedIncreaseMaxSPFX)
	{
		return nullptr;
	}

	UNiagaraComponent* NiagaraComp =
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			LoadedIncreaseMaxSPFX,
			Location,
			Rotation,
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::AutoRelease,
			true
		);

	if (IsValid(NiagaraComp))
	{
		ActiveEffects.AddUnique(
			TWeakObjectPtr<UNiagaraComponent>(NiagaraComp)
		);
	}

	return NiagaraComp;
}

// 防御力増加効果を必要に応じて読み込み、指定位置へプール方式で生成する。
UNiagaraComponent* UEffectSubsystem::SpawnIncreaseDefFX(const FVector& Location, const FRotator& Rotation)
{
	if (!LoadedIncreaseDefFX)
	{
		const UEffectSettings* FXSettings =
			GetDefault<UEffectSettings>();

		if (!FXSettings)
		{
			return nullptr;
		}

		LoadedIncreaseDefFX =
			FXSettings->IncreaseDefFX.LoadSynchronous();
	}

	if (!LoadedIncreaseDefFX)
	{
		return nullptr;
	}

	UNiagaraComponent* NiagaraComp =
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			LoadedIncreaseDefFX,
			Location,
			Rotation,
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::AutoRelease,
			true
		);

	if (IsValid(NiagaraComp))
	{
		ActiveEffects.AddUnique(
			TWeakObjectPtr<UNiagaraComponent>(NiagaraComp)
		);
	}

	return NiagaraComp;
}

// 速度増加効果を必要に応じて読み込み、指定位置へプール方式で生成する。
UNiagaraComponent* UEffectSubsystem::SpawnIncreaseSpeedFX(const FVector& Location, const FRotator& Rotation)
{
	if (!LoadedIncreaseSpeedFX)
	{
		const UEffectSettings* FXSettings =
			GetDefault<UEffectSettings>();

		if (!FXSettings)
		{
			return nullptr;
		}

		LoadedIncreaseSpeedFX =
			FXSettings->IncreaseSpeedFX.LoadSynchronous();
	}

	if (!LoadedIncreaseSpeedFX)
	{
		return nullptr;
	}

	UNiagaraComponent* NiagaraComp =
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			LoadedIncreaseSpeedFX,
			Location,
			Rotation,
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::AutoRelease,
			true
		);

	if (IsValid(NiagaraComp))
	{
		ActiveEffects.AddUnique(
			TWeakObjectPtr<UNiagaraComponent>(NiagaraComp)
		);
	}

	return NiagaraComp;
}

// 攻撃威力増加効果を必要に応じて読み込み、指定位置へプール方式で生成する。
UNiagaraComponent* UEffectSubsystem::SpawnIncreaseAtkPowerFX(const FVector& Location, const FRotator& Rotation)
{
	if (!LoadedIncreaseAtkPowerFX)
	{
		const UEffectSettings* FXSettings =
			GetDefault<UEffectSettings>();

		if (!FXSettings)
		{
			return nullptr;
		}

		LoadedIncreaseAtkPowerFX =
			FXSettings->IncreaseAtkPowerFX.LoadSynchronous();
	}

	if (!LoadedIncreaseAtkPowerFX)
	{
		return nullptr;
	}

	UNiagaraComponent* NiagaraComp =
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			LoadedIncreaseAtkPowerFX,
			Location,
			Rotation,
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::AutoRelease,
			true
		);

	if (IsValid(NiagaraComp))
	{
		ActiveEffects.AddUnique(
			TWeakObjectPtr<UNiagaraComponent>(NiagaraComp)
		);
	}

	return NiagaraComp;
}

// 歪み衝撃効果を必要に応じて読み込み、指定位置へプール方式で生成する。
UNiagaraComponent* UEffectSubsystem::SpawnDistortionShockFX(const FVector& Location, const FRotator& Rotation)
{
	if (!LoadedDistortionShockFX)
	{
		const UEffectSettings* FXSettings =
			GetDefault<UEffectSettings>();

		if (!FXSettings)
		{
			return nullptr;
		}

		LoadedDistortionShockFX =
			FXSettings->DistortionFX.LoadSynchronous();
	}

	if (!LoadedDistortionShockFX)
	{
		return nullptr;
	}

	UNiagaraComponent* NiagaraComp =
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			LoadedDistortionShockFX,
			Location,
			Rotation,
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::AutoRelease,
			true
		);

	if (IsValid(NiagaraComp))
	{
		ActiveEffects.AddUnique(
			TWeakObjectPtr<UNiagaraComponent>(NiagaraComp)
		);
	}

	return NiagaraComp;
}

// 炎状態効果を必要に応じて読み込み、指定位置へプール方式で生成する。
UNiagaraComponent* UEffectSubsystem::SpawnFireStatusFX(const FVector& Location, const FRotator& Rotation)
{
	if (!LoadedFireStatusFX)
	{
		const UEffectSettings* FXSettings =
			GetDefault<UEffectSettings>();

		if (!FXSettings)
		{
			return nullptr;
		}

		LoadedFireStatusFX =
			FXSettings->FireStatusFX.LoadSynchronous();
	}

	if (!LoadedFireStatusFX)
	{
		return nullptr;
	}

	UNiagaraComponent* NiagaraComp =
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			LoadedFireStatusFX,
			Location,
			Rotation,
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::AutoRelease,
			true
		);

	if (IsValid(NiagaraComp))
	{
		ActiveEffects.AddUnique(
			TWeakObjectPtr<UNiagaraComponent>(NiagaraComp)
		);
	}

	return NiagaraComp;
}

// 雷状態効果を必要に応じて読み込み、指定位置へプール方式で生成する。
UNiagaraComponent* UEffectSubsystem::SpawnLightningStatusFX(const FVector& Location, const FRotator& Rotation)
{
	if (!LoadedLightningStatusFX)
	{
		const UEffectSettings* FXSettings =
			GetDefault<UEffectSettings>();

		if (!FXSettings)
		{
			return nullptr;
		}

		LoadedLightningStatusFX =
			FXSettings->LightningStatusFX.LoadSynchronous();
	}

	if (!LoadedLightningStatusFX)
	{
		return nullptr;
	}

	UNiagaraComponent* NiagaraComp =
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			LoadedLightningStatusFX,
			Location,
			Rotation,
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::AutoRelease,
			true
		);

	if (IsValid(NiagaraComp))
	{
		ActiveEffects.AddUnique(
			TWeakObjectPtr<UNiagaraComponent>(NiagaraComp)
		);
	}

	return NiagaraComp;
}

// 風状態効果を必要に応じて読み込み、指定位置へプール方式で生成する。
UNiagaraComponent* UEffectSubsystem::SpawnWindStatusFX(const FVector& Location, const FRotator& Rotation)
{
	if (!LoadedWindStatusFX)
	{
		const UEffectSettings* FXSettings =
			GetDefault<UEffectSettings>();

		if (!FXSettings)
		{
			return nullptr;
		}

		LoadedWindStatusFX =
			FXSettings->WindStatusFX.LoadSynchronous();
	}

	if (!LoadedWindStatusFX)
	{
		return nullptr;
	}

	UNiagaraComponent* NiagaraComp =
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			LoadedWindStatusFX,
			Location,
			Rotation,
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::AutoRelease,
			true
		);

	if (IsValid(NiagaraComp))
	{
		ActiveEffects.AddUnique(
			TWeakObjectPtr<UNiagaraComponent>(NiagaraComp)
		);
	}

	return NiagaraComp;
}
