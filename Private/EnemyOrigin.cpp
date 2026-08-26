// 敵に共通する状態と戦闘を処理する。

#include "EnemyOrigin.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Subsystem/EffectSubsystem.h"
#include "../MyPlayer.h"
#include "../Public/UI/Enemy/ActiveDebuffInfo.h"
#include "../Public/UI/Enemy/StatusBuildUpInfo.h"
#include "BehaviorTree/BlackboardComponent.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
AEnemyOrigin::AEnemyOrigin()
{

	PrimaryActorTick.bCanEverTick = true;

	WindStatusFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WindStatusFX"));
	FireStatusFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireStatusFX"));
	LightningStatusFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LightningStatusFX"));
}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void AEnemyOrigin::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComp)
	{
		AbilitySystemComp->InitAbilityActorInfo(this, this);

		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(
			UEnemyAttributeSet::GetHpAttribute()
		).AddUObject(this, &AEnemyOrigin::OnHpChanged);

		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(
			UEnemyAttributeSet::GetMoveSpeedAttribute()
		).AddUObject(this, &AEnemyOrigin::OnMoveSpeedChanged);

		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(
			UEnemyAttributeSet::GetDefAttribute()
		).AddUObject(this, &AEnemyOrigin::OnDefChanged);
	}

	BaseMoveSpeed = GetCharacterMovement() ? GetCharacterMovement()->MaxWalkSpeed : 600.f;
	IsAlive = true;

	EnemyTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Debuff.Fire")));
	EnemyTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Debuff.Shock")));
	EnemyTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Debuff.Wind")));

	InitializeCharacterMaterials();
}

// 終了時に登録済みイベントやタイマーを解除し、保持参照を整理する。
void AEnemyOrigin::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// 値の変更通知を受け取り、内部状態と関連表示へ反映する。
void AEnemyOrigin::OnHpChanged(const FOnAttributeChangeData& Data)
{
	if(bStatsInitialized == false)
	{
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Enemy HP Changed: %f"), Data.NewValue));
	UpdateHpWidget();
}

// 値の変更通知を受け取り、内部状態と関連表示へ反映する。
void AEnemyOrigin::OnDefChanged(const FOnAttributeChangeData& Data)
{
	if (bStatsInitialized == false)
	{
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Enemy Defense Changed: %f"), Data.NewValue));
}

// 値の変更通知を受け取り、内部状態と関連表示へ反映する。
void AEnemyOrigin::OnMoveSpeedChanged(const FOnAttributeChangeData& Data)
{
	if (bStatsInitialized == false)
	{
		return;
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
		GetCharacterMovement()->MaxAcceleration = Data.NewValue;
	}

	if (GetMesh() && BaseMoveSpeed > 0.f)
	{
		const float AnimRate = Data.NewValue / BaseMoveSpeed;
		GetMesh()->GlobalAnimRateScale = FMath::Clamp(AnimRate, 0.1f, 1.0f);
	}
}

// 現在の体力割合を計算し、頭上の体力表示とボス状態画面へ反映する。
void AEnemyOrigin::UpdateHpWidget()
{
	if(IsAlive == false)
	{
		return;
	}

	if(AttributeSet->GetHp() <= 0.0f)
	{
		AMyPlayer* Player = GetWorld()->GetFirstPlayerController()->GetPawn<AMyPlayer>();
		Player->ClearLockOn();
		OnEnemyDeath.Broadcast(this);
		Death();
	}

	if (!HpWidgetComponent)
	{
		return;
	}

	if (UEnemyHPWidget* EnemyHPWidget = Cast<UEnemyHPWidget>(HpWidgetComponent->GetUserWidgetObject()))
	{
		EnemyHPWidget->UpdateHp();
	}
}

// 登録済みの状態異常効果と変更イベントをすべて解除し、一覧を空にする。
void AEnemyOrigin::RemoveAllDebuffs()
{
	if (!AbilitySystemComp)
	{
		return;
	}

	AbilitySystemComp->RemoveActiveEffectsWithGrantedTags(EnemyTags);

	bStatsInitialized = false;
	IsStatusRimActive = false;

	GetWorldTimerManager().ClearTimer(StatusRimTimerHandle);

	for (UMaterialInstanceDynamic* MID : CharacterMIDs)
	{
		if (!IsValid(MID))
		{
			continue;
		}

		MID->SetScalarParameterValue(
			TEXT("Emissive_Amount_OPBuff"),
			0.f
		);
	}

	SetActorTickEnabled(false);

	GetWorldTimerManager().ClearTimer(
		UIToggleTimer
	);

	if (AbilitySystemComp)
	{
		AbilitySystemComp
			->GetGameplayAttributeValueChangeDelegate(
				UEnemyAttributeSet::GetHpAttribute()
			)
			.RemoveAll(this);

		AbilitySystemComp
			->GetGameplayAttributeValueChangeDelegate(
				UEnemyAttributeSet::GetMoveSpeedAttribute()
			)
			.RemoveAll(this);

		AbilitySystemComp
			->GetGameplayAttributeValueChangeDelegate(
				UEnemyAttributeSet::GetDefAttribute()
			)
			.RemoveAll(this);

		AbilitySystemComp->CancelAllAbilities();


		AbilitySystemComp
			->RemoveActiveEffectsWithGrantedTags(
				EnemyTags
			);
	}

	ActiveDebuffs.Reset();
	ActiveStatusBuildUps.Reset();


	CharacterMIDs.Reset();
}


// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void AEnemyOrigin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	for (int32 i = ActiveDebuffs.Num() - 1; i >= 0; --i)
	{
		ActiveDebuffs[i].RemainingTime -= DeltaTime;

		if (CurrFireThreshold > 0.f)
		{
			CurrFireThreshold -= DeltaTime;
		}
		if (CurrWindThreshold > 0.f)
		{
			CurrWindThreshold -= DeltaTime;
		}
		if (CurrLightningThreshold > 0.f)
		{
			CurrLightningThreshold -= DeltaTime;
		}

		if (ActiveDebuffs[i].RemainingTime <= 0.f)
		{
			if (ActiveDebuffs[i].State == (int32)ECharacterState::Fire)
			{
				CurrFireThreshold = 0.f;
			}
			else if (ActiveDebuffs[i].State == (int32)ECharacterState::Wind)
			{
				CurrWindThreshold = 0.f;
			}
			else if (ActiveDebuffs[i].State == (int32)ECharacterState::Lightning)
			{
				CurrLightningThreshold = 0.f;
			}

			ActiveDebuffs.RemoveAt(i);
		}
	}

	UpdateStatusRimLight(DeltaTime);
}


// 移動・視点・戦闘に使用する入力と処理関数を関連付ける。
void AEnemyOrigin::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// ラグドール移行後にアニメーション更新を停止し、物理姿勢だけを維持する。
void AEnemyOrigin::DisableAnimationAfterRagdoll()
{
	;
}

// 有効デバフを取得し、呼び出し元へ返す。
const TArray<FActiveDebuffInfo>& AEnemyOrigin::GetActiveDebuffs() const
{

	return ActiveDebuffs;
}

// 有効状態蓄積を取得し、呼び出し元へ返す。
const TArray<FStatusBuildUpInfo>& AEnemyOrigin::GetActiveStatusBuildUps() const
{

	return ActiveStatusBuildUps;
}

// 同じ状態異常があれば継続時間を更新し、なければ新たに登録する。
void AEnemyOrigin::AddOrRefreshDebuff(const FGameplayTag& DebuffTag, float CurrDuration, float WholeDuration, ECharacterState State, UTexture2D* IconTexture)
{
	for (int32 i = 0; i < ActiveDebuffs.Num(); i++)
	{
		if (ActiveDebuffs[i].DebuffTag == DebuffTag)
		{
			ActiveDebuffs[i].TotalDuration = WholeDuration;
			ActiveDebuffs[i].RemainingTime = CurrDuration;
			ActiveDebuffs[i].IconTexture = IconTexture;

			if (HpWidgetComponent != nullptr)
			{
				UEnemyHPWidget* EnemyHPWidget = Cast<UEnemyHPWidget>(HpWidgetComponent->GetUserWidgetObject());

				if (EnemyHPWidget != nullptr)
				{
					EnemyHPWidget->RefreshDebuffs();
				}
			}

			if (BossHpWidgetInstance != nullptr)
			{
				BossHpWidgetInstance->RefreshDebuffs();
			}

			if (ActiveDebuffs[i].TotalDuration <= ActiveDebuffs[i].RemainingTime)
			{
				ActiveDebuffs.RemoveAt(i);
				break;
			}
			else
				return;
		}
	}

	FActiveDebuffInfo NewDebuff;
	NewDebuff.DebuffTag = DebuffTag;
	NewDebuff.TotalDuration = WholeDuration;
	NewDebuff.RemainingTime = CurrDuration;
	NewDebuff.State = (int32)State;
	NewDebuff.IconTexture = IconTexture;

	ActiveDebuffs.Add(NewDebuff);

	if (ActiveDebuffs.Num() > 0)
	{
		float RemainingTime = 0.f;

		for (int i = 0; i < ActiveDebuffs.Num(); i++)
		{
			RemainingTime += ActiveDebuffs[i].RemainingTime;
		}

		StartStatusRimLight(
			GetRimLightColor(),
			RemainingTime
		);
	}

	if (HpWidgetComponent != nullptr)
	{
		UEnemyHPWidget* EnemyHPWidget = Cast<UEnemyHPWidget>(HpWidgetComponent->GetUserWidgetObject());

		if (EnemyHPWidget != nullptr)
		{
			EnemyHPWidget->RefreshDebuffs();
		}
	}

	if (BossHpWidgetInstance != nullptr)
	{
		BossHpWidgetInstance->RefreshDebuffs();
	}
}

// 指定タグと一致する状態異常効果および表示情報を削除する。
void AEnemyOrigin::RemoveDebuffByTag(const FGameplayTag& DebuffTag)
{
	ActiveDebuffs.RemoveAll(
		[&](const FActiveDebuffInfo& Debuff)
		{
			if (Debuff.DebuffTag == DebuffTag)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow,
					FString::Printf(TEXT("Removed Debuff: %s"), *DebuffTag.ToString()));
				return true;
			}
			return false;
		}
	);
}

// キャラクターメッシュの動的マテリアルを生成し、元の色と発光値を保存する。
void AEnemyOrigin::InitializeCharacterMaterials()
{
	USkeletalMeshComponent* MeshComp = GetMesh();

	if (!IsValid(MeshComp))
	{
		return;
	}

	CharacterMIDs.Empty();

	const int32 MaterialCount = MeshComp->GetNumMaterials();

	for (int32 Index = 0; Index < MaterialCount; ++Index)
	{
		UMaterialInstanceDynamic* MID =
			MeshComp->CreateDynamicMaterialInstance(Index);

		if (!IsValid(MID))
		{
			continue;
		}

		MID->SetScalarParameterValue(
			TEXT("Emissive_Amount_OPBuff"),
			0.f
		);

		CharacterMIDs.Add(MID);
	}
}

// 輪郭弱を取得し、呼び出し元へ返す。
FLinearColor AEnemyOrigin::GetRimLightColor()
{
	FLinearColor color = FLinearColor(0, 0, 0);

	if (ActiveDebuffs.Num() > 1)
	{
		int DebuffCnt = 0;

		for (int i = 0; i < ActiveDebuffs.Num(); i++)
		{
			if (ActiveDebuffs[i].State == (int32)ECharacterState::Normal)
			{
				continue;
			}

			DebuffCnt++;
		}

		if (DebuffCnt > 1)
		{
			color = FLinearColor(0.5f, 0.1f, 0.5f);
			return color;
		}
	}

	switch (ActiveDebuffs[0].State)
	{
	case (int32)ECharacterState::Fire:
		color = FLinearColor(0.5f, 0.1f, 0.1f);
		break;
	case (int32)ECharacterState::Lightning:
		color = FLinearColor(0.5f, 0.5f, 0.1f);
		break;
	case (int32)ECharacterState::Wind:
		color = FLinearColor(0.1f, 0.1f, 0.5f);
		break;
	}

	return color;
}

// 状態異常の種類に応じた色と継続時間で、輪郭発光を開始する。
void AEnemyOrigin::StartStatusRimLight(const FLinearColor& RimColor, float Duration)
{
	IsStatusRimActive = true;
	RimElapsedTime = 0.f;

	for (UMaterialInstanceDynamic* MID : CharacterMIDs)
	{
		if (!IsValid(MID))
		{
			continue;
		}

		MID->SetVectorParameterValue(
			TEXT("PrimeBuffColor"),
			RimColor
		);

		MID->SetScalarParameterValue(
			TEXT("Emissive_Amount_OPBuff"),
			5.f
		);
	}

	GetWorldTimerManager().ClearTimer(StatusRimTimerHandle);

	if (Duration > 0.f)
	{
		GetWorldTimerManager().SetTimer(
			StatusRimTimerHandle,
			this,
			&AEnemyOrigin::AdjustStatusRimLight,
			Duration,
			false
		);
	}
}

// 有効な状態異常を確認し、表示する輪郭発光の状態を決定する。
void AEnemyOrigin::AdjustStatusRimLight()
{
	if (ActiveDebuffs.IsEmpty() == false)
	{
		for (UMaterialInstanceDynamic* MID : CharacterMIDs)
		{
			if (!IsValid(MID))
			{
				continue;
			}

			MID->SetVectorParameterValue(
				TEXT("PrimeBuffColor"),
				GetRimLightColor()
			);
		}

		return;
	}

	IsStatusRimActive = false;
	RimElapsedTime = 0.f;

	GetWorldTimerManager().ClearTimer(StatusRimTimerHandle);

	for (UMaterialInstanceDynamic* MID : CharacterMIDs)
	{
		if (!IsValid(MID))
		{
			continue;
		}

		MID->SetScalarParameterValue(
			TEXT("Emissive_Amount_OPBuff"),
			0.f
		);
	}
}

// 残り時間に応じて、輪郭発光の強度と色を毎フレーム更新する。
void AEnemyOrigin::UpdateStatusRimLight(float DeltaTime)
{
	if (IsStatusRimActive == false)
	{
		return;
	}

	if (ActiveDebuffs.IsEmpty())
	{
		for (UMaterialInstanceDynamic* MID : CharacterMIDs)
		{
			if (!IsValid(MID))
			{
				continue;
			}

			MID->SetVectorParameterValue(
				TEXT("PrimeBuffColor"),
				FLinearColor(0, 0, 0)
			);
		}

		IsStatusRimActive = false;

		return;
	}

	AdjustStatusRimLight();

	RimElapsedTime += DeltaTime;

	const float SineValue =
		FMath::Sin(RimElapsedTime * RimPulseSpeed);


	const float Alpha = (SineValue + 1.f) * 0.5f;

	const float CurrentIntensity = FMath::Lerp(
		RimMinIntensity,
		RimMaxIntensity,
		Alpha
	);

	for (UMaterialInstanceDynamic* MID : CharacterMIDs)
	{
		if (!IsValid(MID))
		{
			continue;
		}

		MID->SetScalarParameterValue(
			TEXT("Emissive_Amount_OPBuff"),
			CurrentIntensity
		);
	}
}

// 能力システム構成要素を取得し、呼び出し元へ返す。
UAbilitySystemComponent* AEnemyOrigin::GetAbilitySystemComponent() const
{
	return AbilitySystemComp;
}
