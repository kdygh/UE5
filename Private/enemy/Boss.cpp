// 敵固有の行動と戦闘を処理する。

#include "Enemy/Boss.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "../../MyAnimInstancePj26.h"
#include <Kismet/GameplayStatics.h>
#include "Subsystem/EffectSubsystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Data/EnemyInfo.h"
#include "BrainComponent.h"
#include "../../MyPlayer.h"
#include "AttributeSet/EnemyAttributeSet.h"
#include "AttributeSet/PlayerAttributeSet.h"
#include "Subsystem/EffectActorSubsystem.h"
#include "Subsystem/GameFlowSubsystem.h"
#include "Subsystem/GameAudioSubsystem.h"
#include "enemy/LaserCaster.h"
#include "Objects/ShoryukenExplosion.h"
#include "../../Public/MySword.h"
#include "../../Public/UI/Enemy/BossStatusWidget.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
ABoss::ABoss()
{

	PrimaryActorTick.bCanEverTick = true;

	AtkRange = 1000.f;

	AbilitySystemComp = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	AttributeSet = CreateDefaultSubobject<UEnemyAttributeSet>(TEXT("AttributeSet"));
}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void ABoss::BeginPlay()
{
	Super::BeginPlay();

	AnimInst = Cast<UMyAnimInstancePj26>(GetMesh()->GetAnimInstance());

	if (EnemyInfoTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyInfoTable: %s"), *EnemyInfoTable->GetName());

		static const FString ContextString(TEXT("Enemy Info Context"));
		FName RowName = TEXT("Boss");

		FEnemyInfo* EnemyInfo = EnemyInfoTable->FindRow<FEnemyInfo>(RowName, ContextString);

		if (EnemyInfo)
		{
			UE_LOG(LogTemp, Log, TEXT("EnemyName: %s"), *EnemyInfo->EnemyName.ToString());
			UE_LOG(LogTemp, Log, TEXT("AtkPower: %f"), EnemyInfo->AtkPower);
			UE_LOG(LogTemp, Log, TEXT("HP: %f"), EnemyInfo->HP);
			UE_LOG(LogTemp, Log, TEXT("SP: %f"), EnemyInfo->SP);

			MaxHP = EnemyInfo->HP;
			AtkPower = EnemyInfo->AtkPower;
			CurrHP = MaxHP;

			AttributeSet->SetMaxHp(MaxHP);
			AttributeSet->SetHp(CurrHP);
			AttributeSet->SetMaxSp(EnemyInfo->SP);
			AttributeSet->SetSp(EnemyInfo->SP);
			AttributeSet->SetAtkPower(AtkPower);
			AttributeSet->SetDef(EnemyInfo->Def);
			AttributeSet->SetMoveSpeed(GetCharacterMovement()->MaxWalkSpeed);


			UE_LOG(LogTemp, Log, TEXT("AFTER SP: %f"), AttributeSet->GetSp());

			IsAlive = true;
			bStatsInitialized = true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("FindRow failed. RowName: %s"), *RowName.ToString());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyInfoTable is nullptr! Class: %s"), *GetClass()->GetName());
	}

	if (BossHpWidgetClass)
	{
		BossHpWidgetInstance = CreateWidget<UBossStatusWidget>(GetWorld(), BossHpWidgetClass);
		if (BossHpWidgetInstance)
		{
			BossHpWidgetInstance->AddToViewport();
			BossHpWidgetInstance->SetOwnerEnemy(this);
		}
	}
}

// 生存状態を解除し、移動・衝突・戦闘を停止した後に死亡演出を開始する。
void ABoss::Death()
{
	UGameFlowSubsystem* GameFlowSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UGameFlowSubsystem>();
	if (GameFlowSubsystem)
	{
		GameFlowSubsystem->ShowBossCleared();
	}

	IsAlive = false;
	IsStunned = false;
	IsTraceAttackActiveL = false;
	IsTraceAttackActiveR = false;

	RemoveAllDebuffs();

	if (BossHpWidgetClass && BossHpWidgetInstance)
	{
		BossHpWidgetInstance->UpdateHp();
		BossHpWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
	}


	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		if (AICon->GetBrainComponent())
		{
			AICon->GetBrainComponent()->StopLogic(TEXT("Enemy Dead"));
		}

		AICon->StopMovement();
		AICon->UnPossess();
	}

	AnimInst->StopAllMontages(0.0f);
	AnimInst->SetIgnoreRootMotion(false);
	AnimInst->Montage_Play(DeathAnimMontage);
	AnimInst->Montage_JumpToSection("Death0");


	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle, [this]() {

			if (UEffectSubsystem* EffectSubsystem = GetWorld()->GetSubsystem<UEffectSubsystem>())
			{
				EffectSubsystem->SpawnExplosionFX(GetActorLocation(), FRotator::ZeroRotator);
			}

			UGameAudioSubsystem* AudioSubsystem = GetGameInstance()->GetSubsystem<UGameAudioSubsystem>();
			if (AudioSubsystem)
			{
				AudioSubsystem->PlayExplosionSound(GetActorLocation());
			}

			Destroy();
		}, 2.0f, false
	);
}

// 衝突相手とダメージ量を確認し、プレイヤーへ通常の衝突ダメージを適用する。
void ABoss::CalCollisionDamage(float DamageAmount)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("DamageAmount: %f"), DamageAmount));
	IsTraceAttackActiveL = false;
	IsTraceAttackActiveR = false;
	AttributeSet->SetHp(FMath::Clamp(AttributeSet->GetHp() - DamageAmount, 0.f, AttributeSet->GetMaxHp()));
	SetHP(AttributeSet->GetHp() - DamageAmount);
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Current HP: %f"), AttributeSet->GetHp()));

	if (IsStunned)
	{
		return;
	}

	AAIController* controller = Cast<AAIController>(GetController());
	if (controller)
	{
		controller->GetBlackboardComponent()->SetValueAsBool(FName("didNoticePlayer"), true);
	}
}

// 衝突相手を確認し、プレイヤーへ属性またはスタミナに関するダメージを適用する。
void ABoss::CalCollisionDamageSP(float DamageAmount)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("DamageAmount: %f"), DamageAmount));
	AttributeSet->SetSp(FMath::Clamp(AttributeSet->GetSp() - DamageAmount, 0.f, AttributeSet->GetMaxSp()));
	SetSP(AttributeSet->GetSp());
}

// 気絶状態へ切り替え、現在の行動を停止して気絶演出を開始する。
void ABoss::Stun()
{
	AnimInst->Montage_Play(m_animMontage);
	AnimInst->Montage_JumpToSection("Stun");
}

// 体力割合を取得し、呼び出し元へ返す。
float ABoss::GetHpPercent() const
{
	return AttributeSet->GetMaxHp() > 0.f ? AttributeSet->GetHp() / AttributeSet->GetMaxHp() : 0.f;
}

// スタミナ割合を取得し、呼び出し元へ返す。
float ABoss::GetSpPercent() const
{
	return AttributeSet->GetMaxSp() > 0.f ? AttributeSet->GetSp() / AttributeSet->GetMaxSp() : 0.f;
}

// 体力を指定された値へ更新する。
void ABoss::SetHP(float NewHP)
{
	if (BossHpWidgetInstance)
	{
		BossHpWidgetInstance->UpdateHp();
	}
}

// スタミナを指定された値へ更新する。
void ABoss::SetSP(float NewSP)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("SetSP called with NewSP: %f"), NewSP));

	if (AttributeSet->GetSp() <= 0.f)
	{
		Stun();
	}

	if (BossHpWidgetInstance)
	{
		BossHpWidgetInstance->UpdateSp();
	}
}


// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AnimInst->IsInAir = GetCharacterMovement()->IsFalling();

	FVector Vel = GetVelocity();
	Vel.Z = 0.f;
	AnimInst->Speed = Vel.Size();

	if (IsTraceAttackActiveL)
	{
		PerformWeaponTraceL();
	}

	if (IsTraceAttackActiveR)
	{
		PerformWeaponTraceR();
	}
}


// 移動・視点・戦闘に使用する入力と処理関数を関連付ける。
void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void ABoss::OnFollowPlayer(bool _isPlayerInSight)
{
}

// 現在の戦闘状態と入力条件を確認し、対応する攻撃処理を実行する。
void ABoss::MeleeAttack()
{
	if (AMyPlayer* Player = Cast<AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		if (Player->GetIsAlive() == false)
		{
			return;
		}
	}

	AAIController* AICon = Cast<AAIController>(GetController());

	bool isPlayerClose = AICon->GetBlackboardComponent()->GetValueAsBool("isPlayerinAtkRange");

	float Rand = FMath::FRand();
	int32 RandomIndex = (Rand < 0.8f) ? 0 : 1;

	if (isPlayerClose == true)
	{
		if (AnimInst && m_animMontage)
		{
			FName SectionName = (RandomIndex == 0) ? "Atk1" : "Shoryuken";
			AnimInst->Montage_Play(m_animMontage);
			AnimInst->Montage_JumpToSection(SectionName);
		}
	}
	else
	{
		if (AnimInst && DashMontage)
		{
			FName SectionName = (RandomIndex == 0) ? "Dash" : "Cast";
			AnimInst->Montage_Play(DashMontage);
			AnimInst->Montage_JumpToSection(SectionName);
		}
	}
}

// アニメーション通知名に応じて、攻撃判定・回避・パリィ・死亡状態を切り替える。
void ABoss::OnAnimAction(FName ActionName)
{
	if (ActionName == "LaunchAtk")
	{
		if (AAIController* AICon = Cast<AAIController>(GetController()))
		{
			AICon->StopMovement();
		}

		AMyPlayer* Player = Cast<AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (!Player)
		{
			return;
		}

		FVector Dir = Player->GetActorLocation() - GetActorLocation();
		Dir.Z = 0.0f;

		if (!Dir.IsNearlyZero())
		{
			FRotator TargetRot = Dir.Rotation();
			SetActorRotation(TargetRot);
		}

		AnimInst->SetIgnoreRootMotion(IsAttacking);
		LaunchCharacter(GetActorForwardVector() * 4000.f, true, true);
	}
	else if (ActionName == "DashStart")
	{
		if (AAIController* AICon = Cast<AAIController>(GetController()))
		{
			AICon->StopMovement();
		}

		AMyPlayer* Player = Cast<AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (!Player)
		{
			return;
		}

		FVector Dir = Player->GetActorLocation() - GetActorLocation();
		Dir.Z = 0.0f;

		if (!Dir.IsNearlyZero())
		{
			FRotator TargetRot = Dir.Rotation();
			SetActorRotation(TargetRot);
		}

		bIsDashing = true;
		AnimInst->SetIgnoreRootMotion(bIsDashing);
		LaunchCharacter(GetActorForwardVector() * 20000.f, true, true);
	}
	else if (ActionName == "DashEnd")
	{
		bIsDashing = false;
		AnimInst->SetIgnoreRootMotion(bIsDashing);
	}
	else if (ActionName == "AtkEnd")
	{
		IsAttacking = false;
		AnimInst->SetIgnoreRootMotion(IsAttacking);
	}
	else if (ActionName == "AtkStart")
	{
		IsAttacking = true;
		AnimInst->SetIgnoreRootMotion(IsAttacking);
	}
	else if (ActionName == "LAtkCollDisable")
	{
		EndTraceAttackL();
		bIsHitPlayer = false;
	}
	else if (ActionName == "LAtkCollEnable")
	{
		StartTraceAttackL();
	}
	else if (ActionName == "RAtkCollDisable")
	{
		EndTraceAttackR();
		bIsHitPlayer = false;
	}
	else if (ActionName == "RAtkCollEnable")
	{
		StartTraceAttackR();
		IsTraceAttackActiveR = true;
	}
	else if (ActionName == "StunEnd")
	{
		IsStunned = false;
		AttributeSet->SetSp(AttributeSet->GetMaxSp());
		SetSP(AttributeSet->GetSp());
	}
	else if (ActionName == "StunStart")
	{
		IsStunned = true;
	}
	else if (ActionName == "Cast")
	{
		if (USkeletalMeshComponent* MeshComp = GetMesh())
		{
			const FName SocketName = TEXT("FX_handDir_r");

			const FVector SpawnLocation = FVector(MeshComp->GetSocketLocation(SocketName).X,
				MeshComp->GetSocketLocation(SocketName).Y,
				MeshComp->GetSocketLocation(SocketName).Z + 200.f);

			AMyPlayer* Player = GetWorld()->GetFirstPlayerController()->GetPawn<AMyPlayer>();
			FVector Dir = Player->GetActorLocation() - GetActorLocation();
			Dir.Z = 0.0f;

			FRotator TargetRot = Dir.Rotation();
			SetActorRotation(TargetRot);

			if (UEffectActorSubsystem* Pool = GetWorld()->GetSubsystem<UEffectActorSubsystem>())
			{
			    FTransform SpawnTransform;
			    SpawnTransform.SetLocation(SpawnLocation);
				SpawnTransform.SetRotation(TargetRot.Quaternion());

			    Pool->AcquireActor(LaserCasterClass, SpawnTransform, GetActorForwardVector());
			}
		}
	}
	else if (ActionName == "ShoryukenExplosion")
	{
		if (UEffectActorSubsystem* Pool = GetWorld()->GetSubsystem<UEffectActorSubsystem>())
		{
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(GetActorLocation());

			Pool->AcquireActor(ShoryukenExplosionClass, SpawnTransform, GetActorForwardVector());
		}
	}
	else if (ActionName == "EnableRagdoll")
	{

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);


		USkeletalMeshComponent* MeshComp = GetMesh();
		if (MeshComp)
		{
			MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));


			MeshComp->SetSimulatePhysics(true);
			MeshComp->SetAllBodiesSimulatePhysics(true);
			MeshComp->SetEnableGravity(true);
			MeshComp->WakeAllRigidBodies();
			MeshComp->bBlendPhysics = true;
		}

		// 現在は使用していない処理。
		//GetWorldTimerManager().SetTimerForNextTick(this, &ABoss::DisableAnimationAfterRagdoll);
	}

}

// 現在の戦闘状態と入力条件を確認し、対応する攻撃処理を実行する。
void ABoss::DashAttack()
{
	bIsDashing = true;
	if (AnimInst && DashMontage)
	{
		AnimInst->Montage_Play(DashMontage);
	}
}

// ラグドール移行後にアニメーション更新を停止し、物理姿勢だけを維持する。
void ABoss::DisableAnimationAfterRagdoll()
{
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetAnimationMode(EAnimationMode::AnimationCustomMode);
		MeshComp->SetEnableAnimation(false);
	}
}

// 左武器の衝突判定を有効にし、直前のソケット位置を記録する。
void ABoss::StartTraceAttackL()
{
	IsTraceAttackActiveL = true;
	HitActors.Reset();

	if (GetMesh())
	{
		PrevStart = GetMesh()->GetSocketLocation(TraceStartSocketL);
	}
}

// 左武器の衝突判定を終了し、命中対象の記録を整理する。
void ABoss::EndTraceAttackL()
{
	IsTraceAttackActiveL = false;
	HitActors.Reset();
}

// 左武器の前回・現在位置をスイープし、攻撃の衝突を検出する。
void ABoss::PerformWeaponTraceL()
{
	if (!GetMesh())
	{
		return;
	}

	const FVector CurrentStart = GetMesh()->GetSocketLocation(TraceStartSocketL);

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

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor)
		{
			continue;
		}

		if (AMyPlayer* Player = Cast<AMyPlayer>(HitActor))
		{
			if (Player->GetIsParrying())
			{
				Player->SetParrying(Hit.ImpactPoint);

				if (AttributeSet->GetSp() <= 0.f)
				{
					Stun();
				}

				EndTraceAttackL();
				EndTraceAttackR();
				return;
			}
			if (Player->GetIsEvading())
			{
				return;
			}

			if (bIsHitPlayer == true)
			{
				EndTraceAttackL();
				EndTraceAttackR();
				return;
			}

			bIsHitPlayer = true;
			Player->CalCollisionDamage(AttributeSet->GetAtkPower(), this);
		}
	}

	PrevStart = CurrentStart;
}

// 右武器の衝突判定を有効にし、直前のソケット位置を記録する。
void ABoss::StartTraceAttackR()
{
	IsTraceAttackActiveR = true;
	HitActors.Reset();

	if (GetMesh())
	{
		PrevStart = GetMesh()->GetSocketLocation(TraceStartSocketR);
	}
}

// 右武器の衝突判定を終了し、命中対象の記録を整理する。
void ABoss::EndTraceAttackR()
{
	IsTraceAttackActiveR = false;
	HitActors.Reset();
}

// 右武器の前回・現在位置をスイープし、攻撃の衝突を検出する。
void ABoss::PerformWeaponTraceR()
{
	if (!GetMesh())
	{
		return;
	}

	const FVector CurrentStart = GetMesh()->GetSocketLocation(TraceStartSocketR);

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

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor)
		{
			continue;
		}

		if (AMyPlayer* Player = Cast<AMyPlayer>(HitActor))
		{
			if (Player->GetIsParrying())
			{
				Player->SetParrying(Hit.ImpactPoint);


				if (AttributeSet->GetSp() <= 0.f)
				{
					Stun();
				}

				if (bIsDashing == true)
				{
					bIsDashing = false;
					AnimInst->SetIgnoreRootMotion(bIsDashing);
					AnimInst->Montage_Play(DashMontage);
					AnimInst->Montage_JumpToSection("DashEnd");
				}

				EndTraceAttackL();
				EndTraceAttackR();

				return;
			}
			if (Player->GetIsEvading())
			{
				return;
			}

			if (bIsHitPlayer == true)
			{
				EndTraceAttackL();
				EndTraceAttackR();
				return;
			}

			bIsHitPlayer = true;
			Player->CalCollisionDamage(AttributeSet->GetAtkPower(), this);
		}
	}

	PrevStart = CurrentStart;
}
