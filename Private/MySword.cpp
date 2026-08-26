// 武器の当たり判定と属性攻撃を処理する。

#include "MySword.h"
#include "Components/CapsuleComponent.h"
#include <Kismet/GameplayStatics.h>
#include "EnemyOrigin.h"
#include "../MyPlayer.h"
#include "Subsystem/EffectSubsystem.h"
#include "Subsystem/CombatFeedbackSubsystem.h"
#include "Subsystem/GameAudioSubsystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AttributeSet/EnemyAttributeSet.h"
#include "AttributeSet/PlayerAttributeSet.h"
#include "UI/Enemy/ActiveDebuffInfo.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
AMySword::AMySword()
{

	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwordMesh"));
	SwordMesh->SetupAttachment(Root);
}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void AMySword::BeginPlay()
{
	Super::BeginPlay();
	DisableWeaponCollision();
}

// 構成要素の生成完了後に、相互参照とイベントを接続する。
void AMySword::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}


// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void AMySword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(IsTraceAttackActive == true)
	{
		PerformWeaponTrace();
	}
}

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void AMySword::EnableWeaponCollision()
{
	// 現在は使用していない処理。
	//Weapon->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//Weapon->SetGenerateOverlapEvents(true);
}

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void AMySword::DisableWeaponCollision()
{
	// 現在は使用していない処理。
	//Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//Weapon->SetGenerateOverlapEvents(false);
}

// 攻撃開始時に武器の衝突判定を有効にし、直前のソケット位置を記録する。
void AMySword::StartTraceAttack()
{
	IsTraceAttackActive = true;
	// 現在は使用していない処理。
	//HitActors.Reset();

	if (SwordMesh)
	{
		PrevStart = SwordMesh->GetSocketLocation(TraceStartSocket);
		PrevEnd = SwordMesh->GetSocketLocation(TraceEndSocket);

		UE_LOG(LogTemp, Warning, TEXT("StartTraceAttack: PrevStart = %s, PrevEnd = %s"), *PrevStart.ToString(), *PrevEnd.ToString());
	}
}

// 攻撃終了時に武器の衝突判定を停止し、命中対象の記録を初期化する。
void AMySword::EndTraceAttack()
{
	IsTraceAttackActive = false;
	HitActors.Reset();
}

// 前回と現在の武器位置の間をスイープし、高速な攻撃の衝突を検出する。
void AMySword::PerformWeaponTrace()
{
	if (!SwordMesh)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("No SwordMesh"));
		return;
	}

	const FVector CurrentStart = SwordMesh->GetSocketLocation(TraceStartSocket);
	const FVector CurrentEnd = SwordMesh->GetSocketLocation(TraceEndSocket);

	TArray<FHitResult> HitResults;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	if (AActor* OwnerActor = GetOwner())
	{
		ActorsToIgnore.Add(OwnerActor);
	}


	UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(),
		PrevStart,
		CurrentStart,
		TraceRadius,
		UEngineTypes::ConvertToTraceType(TraceChannel),
		false,
		ActorsToIgnore,
		bDrawDebugTrace ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		HitResults,
		true
	);

	AMyPlayer* Player = Cast<AMyPlayer>(GetOwner());

	if (Player == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("No Player"));
		return;
	}

	UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent();
	const UPlayerAttributeSet* AttrSet = ASC->GetSet<UPlayerAttributeSet>();
	UEffectSubsystem* EffectSubsystem = GetWorld()->GetSubsystem<UEffectSubsystem>();
	UCombatFeedbackSubsystem* CombatFeedbackSubsystem = GetWorld()->GetSubsystem<UCombatFeedbackSubsystem>();
	UGameAudioSubsystem* AudioSubsystem = GetGameInstance()->GetSubsystem<UGameAudioSubsystem>();

	int32 HitCnt = 0;

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor)
		{
			continue;
		}

		if (HitActors.Contains(HitActor))
		{
			continue;
		}

		HitActors.Add(HitActor);

		if (AEnemyOrigin* Enemy = Cast<AEnemyOrigin>(HitActor))
		{
			if (Enemy->GetIsAlive() == true)
			{
				++MaxHitSoundCnt;
				bIsHitEnemy = true;
				EffectSubsystem->SpawnHitFX(Hit.Location, FRotator::ZeroRotator);
				CombatFeedbackSubsystem->PlayHitStop(HitStopThreshhold);
				CombatFeedbackSubsystem->PlayCameraShake(AtkCameraShakeClass, CamerashakeScale);

				if (HitCnt <= MaxHitSoundCnt)
				{
					AudioSubsystem->PlayHitSound(Hit.Location);
				}

				if (AttrSet)
				{
					float damage = AttrSet->GetAtkPower() * (100.0f / (100.0f + Enemy->GetAttributeSet()->GetDef()));
					Player->ApplyDamageToTarget(damage, Enemy);


					if(Player->GetIsStaminaEnough() == true)
					{
						ApplyAtkAttribute(Player, Enemy, EffectSubsystem, AudioSubsystem);
					}

					Enemy->SetHP(0.0f);
					Enemy->Damaged();
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("No Player Attribute Set"));
				}
			}
			else
			{
				Player->ClearLockOn();
			}
		}
	}

	PrevStart = CurrentStart;
}

// 現在の攻撃属性と蓄積値を確認し、対象へ属性ダメージと状態異常を適用する。
void AMySword::ApplyAtkAttribute(AMyPlayer* Player, AEnemyOrigin* Enemy, UEffectSubsystem* EffectSubsystem, UGameAudioSubsystem* AudioSubsystem)
{
	switch (Player->GetCurrentAttackAttribute())
	{
	case EAttackAttribute::None:
		break;
	case EAttackAttribute::Fire:
	{
		EffectSubsystem->SpawnShockwaveFX(Enemy->GetActorLocation(), FRotator::ZeroRotator);
		AudioSubsystem->PlayFireWeaponSwingSound(Enemy->GetActorLocation());

		Enemy->AddFireThreshold(Player->GetFireSP());

		bool IsEnemyDebuffable = true;

		for (int32 i = 0; i < Enemy->GetActiveDebuffs().Num(); i++)
		{
			if (Enemy->GetActiveDebuffs()[i].State == (int32)ECharacterState::Fire)
			{
				IsEnemyDebuffable = false;
				return;
			}
		}

		if (IsEnemyDebuffable == true)
		{
			Enemy->AddOrRefreshDebuff(
				FGameplayTag::RequestGameplayTag(TEXT("State.Debuff.Fire")),
				Enemy->GetCurrFireThreshold(),
				Enemy->GetFireThreshold(),
				ECharacterState::Normal,
				FireIconTexture
			);
		}

		if (Enemy->GetCurrFireThreshold() >= Enemy->GetFireThreshold())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Fire"));
			Enemy->SetCurrFireThreshold(Enemy->GetFireThreshold());
			Player->ApplyFireEffect(Enemy);
			Enemy->AddOrRefreshDebuff(
				FGameplayTag::RequestGameplayTag(TEXT("State.Debuff.Fire")),
				StateDuration,
				StateDuration,
				ECharacterState::Fire,
				FireIconTexture
			);
		}
	}
	break;
	case EAttackAttribute::Lightning:
	{
		EffectSubsystem->SpawnLightningFX(Enemy->GetActorLocation(), FRotator::ZeroRotator);
		AudioSubsystem->PlayLightningWeaponSwingSound(Enemy->GetActorLocation());

		Enemy->AddLightningThreshold(Player->GetLightningSP());

		bool IsEnemyDebuffable = true;

		for (int32 i = 0; i < Enemy->GetActiveDebuffs().Num(); i++)
		{
			if (Enemy->GetActiveDebuffs()[i].State == (int32)ECharacterState::Lightning)
			{
				IsEnemyDebuffable = false;
				return;
			}
		}

		if (IsEnemyDebuffable == true)
		{
			Enemy->AddOrRefreshDebuff(
				FGameplayTag::RequestGameplayTag(TEXT("State.Debuff.Shock")),
				Enemy->GetCurrLightningThreshold(),
				Enemy->GetLightningThreshold(),
				ECharacterState::Normal,
				ShockIconTexture
			);
		}

		if (Enemy->GetCurrLightningThreshold() >= Enemy->GetLightningThreshold())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Shock"));
			Enemy->SetCurrLightningThreshold(Enemy->GetLightningThreshold());
			Player->ApplyLightningEffect(Enemy);
			Enemy->AddOrRefreshDebuff(
				FGameplayTag::RequestGameplayTag(TEXT("State.Debuff.Shock")),
				StateDuration,
				StateDuration,
				ECharacterState::Lightning,
				ShockIconTexture
			);
		}
	}
	break;
	case EAttackAttribute::Wind:
	{
		EffectSubsystem->SpawnWindFX(Enemy->GetActorLocation(), FRotator::ZeroRotator);
		AudioSubsystem->PlayWindWeaponSwingSound(Enemy->GetActorLocation());

		Enemy->AddWindThreshold(Player->GetWindSP());

		bool IsEnemyDebuffable = true;

		for (int32 i = 0; i < Enemy->GetActiveDebuffs().Num(); i++)
		{
			if (Enemy->GetActiveDebuffs()[i].State == (int32)ECharacterState::Wind)
			{
				IsEnemyDebuffable = false;
				return;
			}
		}

		if (IsEnemyDebuffable == true)
		{
			Enemy->AddOrRefreshDebuff(
				FGameplayTag::RequestGameplayTag(TEXT("State.Debuff.Wind")),
				Enemy->GetCurrWindThreshold(),
				Enemy->GetWindThreshold(),
				ECharacterState::Normal,
				WindIconTexture
			);
		}

		if (Enemy->GetCurrWindThreshold() >= Enemy->GetWindThreshold())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Wind"));
			Enemy->SetCurrWindThreshold(Enemy->GetWindThreshold());
			Player->ApplyWindEffect(Enemy);
			Enemy->AddOrRefreshDebuff(
				FGameplayTag::RequestGameplayTag(TEXT("State.Debuff.Wind")),
				StateDuration,
				StateDuration,
				ECharacterState::Wind,
				WindIconTexture
			);
		}
	}
	break;
	}
}


// 重なりを検知した相手の種類を確認し、該当するダメージまたは取得効果を適用する。
void AMySword::OnOverlapsBegin_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ECollisionChannel OtherChannel = OtherComp->GetCollisionObjectType();
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("OnOverlapsBegin_Implementation"));
	switch (OtherChannel)
	{
	case ECC_GameTraceChannel5:
		break;
	}
}

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void AMySword::OnOverlapsEnd_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
