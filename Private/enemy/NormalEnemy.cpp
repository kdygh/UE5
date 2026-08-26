// 敵固有の行動と戦闘を処理する。

#include "enemy/NormalEnemy.h"
#include "EnemyAI/NormalEnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "../../MyAnimInstancePj26.h"
#include <Kismet/GameplayStatics.h>
#include "Subsystem/EffectSubsystem.h"
#include "Subsystem/GameAudioSubsystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Data/EnemyInfo.h"
#include "BrainComponent.h"
#include "../../MyPlayer.h"
#include "AttributeSet/EnemyAttributeSet.h"
#include "AttributeSet/PlayerAttributeSet.h"
#include "../../Public/MySword.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
ANormalEnemy::ANormalEnemy()
{

	PrimaryActorTick.bCanEverTick = true;

	AtkRange = 300.f;


	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	WeaponRight = CreateDefaultSubobject<UCapsuleComponent>(TEXT("WeaponRight"));
	WeaponRight->SetupAttachment(GetMesh(), TEXT("weapon_sword_r"));
	WeaponRight->SetRelativeLocation(FVector::ZeroVector);

	WeaponLeft = CreateDefaultSubobject<UCapsuleComponent>(TEXT("WeaponLeft"));
	WeaponLeft->SetupAttachment(GetMesh(), TEXT("weapon_sword_l"));
	WeaponLeft->SetRelativeLocation(FVector::ZeroVector);

	AbilitySystemComp = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	AttributeSet = CreateDefaultSubobject<UEnemyAttributeSet>(TEXT("AttributeSet"));

	HpWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HpWidgetComponent"));
	HpWidgetComponent->SetupAttachment(GetRootComponent());
	HpWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HpWidgetComponent->SetDrawSize(FVector2D(180.f, 80.f));
	HpWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void ANormalEnemy::BeginPlay()
{
	Super::BeginPlay();

	AnimInst = Cast<UMyAnimInstancePj26>(GetMesh()->GetAnimInstance());

	WeaponRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponRight->SetGenerateOverlapEvents(false);

	WeaponLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponLeft->SetGenerateOverlapEvents(false);

	if (EnemyInfoTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyInfoTable: %s"), *EnemyInfoTable->GetName());

		static const FString ContextString(TEXT("Enemy Info Context"));
		FName RowName = TEXT("NormalEnemy");

		FEnemyInfo* EnemyInfo = EnemyInfoTable->FindRow<FEnemyInfo>(RowName, ContextString);

		if (EnemyInfo)
		{
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

	if (UEnemyHPWidget* EnemyHPWidget = Cast<UEnemyHPWidget>(HpWidgetComponent->GetUserWidgetObject()))
	{
		EnemyHPWidget->SetOwnerEnemy(this);
	}

	AAIController* controller = Cast<AAIController>(GetController());
	if (controller)
	{
		controller->GetBlackboardComponent()->SetValueAsBool(FName("didNoticePlayer"), true);
	}

}

// 生存状態を解除し、移動・衝突・戦闘を停止した後に死亡演出を開始する。
void ANormalEnemy::Death()
{
	IsAlive = false;
	IsStunned = false;
	IsTraceAttackActiveL = false;
	IsTraceAttackActiveR = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(
		ECC_GameTraceChannel4,
		ECR_Ignore);

	RemoveAllDebuffs();

	HpWidgetComponent->SetVisibility(false);
	HpWidgetComponent->Deactivate();

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
		TimerHandle, [this]() {Destroy(); }, 3.0f, false
	);
}

// 衝突相手とダメージ量を確認し、プレイヤーへ通常の衝突ダメージを適用する。
void ANormalEnemy::CalCollisionDamage(float DamageAmount)
{
	UEffectSubsystem* EffectSubsystem = GetWorld()->GetSubsystem<UEffectSubsystem>();
	if (EffectSubsystem)
	{
		EffectSubsystem->SpawnHitFX(GetActorLocation(), FRotator::ZeroRotator);
	}

	IsTraceAttackActiveL = false;
	IsTraceAttackActiveR = false;
	AttributeSet->SetHp(FMath::Clamp(AttributeSet->GetHp() - DamageAmount, 0.f, AttributeSet->GetMaxHp()));
	SetHP(AttributeSet->GetHp() - DamageAmount);
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("CurrentHP: %f"), AttributeSet->GetHp()));

	if(AttributeSet->GetHp() <= 0.f)
	{
		Death();
		return;
	}

	if (IsStunned)
	{
		return;
	}

	AnimInst->SetIgnoreRootMotion(false);
	AnimInst->Montage_Play(DmgAnimMontage);
}

// 衝突相手を確認し、プレイヤーへ属性またはスタミナに関するダメージを適用する。
void ANormalEnemy::CalCollisionDamageSP(float DamageAmount)
{
	WeaponRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponRight->SetGenerateOverlapEvents(false);

	WeaponLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponLeft->SetGenerateOverlapEvents(false);

	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("DamageAmount: %f"), DamageAmount));
	AttributeSet->SetSp(FMath::Clamp(AttributeSet->GetSp() - DamageAmount, 0.f, AttributeSet->GetMaxSp()));
	SetSP(AttributeSet->GetSp());
}

// 被弾状態へ切り替え、攻撃中断・アニメーション・関連演出を処理する。
void ANormalEnemy::Damaged()
{
	EndTraceAttackL();
	EndTraceAttackR();

	if (IsAlive == false)
	{
		return;
	}

	if (IsStunned)
	{
		return;
	}

	AAIController* controller = Cast<AAIController>(GetController());
	if (controller)
	{
		controller->GetBlackboardComponent()->SetValueAsBool(FName("didNoticePlayer"), true);
	}

	AnimInst->SetIgnoreRootMotion(false);
	AnimInst->Montage_Play(DmgAnimMontage);

}

// 気絶状態へ切り替え、現在の行動を停止して気絶演出を開始する。
void ANormalEnemy::Stun()
{
	WeaponRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponRight->SetGenerateOverlapEvents(false);

	WeaponLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponLeft->SetGenerateOverlapEvents(false);

	AnimInst->Montage_Play(AtkMontage);
	AnimInst->Montage_JumpToSection("Stun");
}

// 構成要素の生成完了後に、相互参照とイベントを接続する。
void ANormalEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	GetCapsuleComponent()->OnComponentBeginOverlap.__Internal_AddDynamic(this, &ANormalEnemy::OnOverlapsBegin, FName("OnOverlapsBegin"));
	GetCapsuleComponent()->OnComponentEndOverlap.__Internal_AddDynamic(this, &ANormalEnemy::OnOverlapsEnd, FName("OnOverlapsEnd"));

	WeaponRight->OnComponentBeginOverlap.__Internal_AddDynamic(this, &ANormalEnemy::OnRWeaponOverlapsBegin, FName("OnRWeaponOverlapsBegin"));
	WeaponLeft->OnComponentBeginOverlap.__Internal_AddDynamic(this, &ANormalEnemy::OnLWeaponOverlapsBegin, FName("OnLWeaponOverlapsBegin"));

}

// 対象とプレイヤーの時間倍率を下げ、タイマーで通常速度への復帰を予約する。
void ANormalEnemy::DoHitStop(float Duration)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("DoHitStop"));
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0011f);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,
		FTimerDelegate::CreateUObject(this, &ANormalEnemy::ResetTimeDilation, Duration),
		Duration, false);
}

// 一時的に変更した時間倍率を通常の値へ戻す。
void ANormalEnemy::ResetTimeDilation(float Duration)
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
}

// 体力割合を取得し、呼び出し元へ返す。
float ANormalEnemy::GetHpPercent() const
{
	return AttributeSet->GetMaxHp() > 0.f ? AttributeSet->GetHp() / AttributeSet->GetMaxHp() : 0.f;
}

// スタミナ割合を取得し、呼び出し元へ返す。
float ANormalEnemy::GetSpPercent() const
{
	return AttributeSet->GetMaxSp() > 0.f ? AttributeSet->GetSp() / AttributeSet->GetMaxSp() : 0.f;
}

// 体力を指定された値へ更新する。
void ANormalEnemy::SetHP(float NewHP)
{
	if (UEnemyHPWidget* EnemyHPWidget = Cast<UEnemyHPWidget>(HpWidgetComponent->GetUserWidgetObject()))
	{
		EnemyHPWidget->UpdateHp();
	}
}

// スタミナを指定された値へ更新する。
void ANormalEnemy::SetSP(float NewSP)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("SetSP called with NewSP: %f"), NewSP));

	if (AttributeSet->GetSp() <= 0.f)
	{
		Stun();
	}

	if (UEnemyHPWidget* EnemyHPWidget = Cast<UEnemyHPWidget>(HpWidgetComponent->GetUserWidgetObject()))
	{
		EnemyHPWidget->UpdateSp();
	}
}


// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void ANormalEnemy::Tick(float DeltaTime)
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
	else if(IsTraceAttackActiveR)
	{
		PerformWeaponTraceR();
	}
}


// 移動・視点・戦闘に使用する入力と処理関数を関連付ける。
void ANormalEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// プレイヤーに関するイベントを受け取り、関連状態を更新する。
void ANormalEnemy::OnFollowPlayer(bool _isPlayerInSight)
{
	if (_isPlayerInSight == true)
	{
		GetCharacterMovement()->MaxWalkSpeed = 350.f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 150.f;
	}

	isPlayerInSight = _isPlayerInSight;
}

// 現在の戦闘状態と入力条件を確認し、対応する攻撃処理を実行する。
void ANormalEnemy::MeleeAttack()
{
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance || !AtkMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance or AtkMontage invalid"));
		return;
	}

	float Result = AnimInstance->Montage_Play(AtkMontage);
}

// ラグドール移行後にアニメーション更新を停止し、物理姿勢だけを維持する。
void ANormalEnemy::DisableAnimationAfterRagdoll()
{
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetAnimationMode(EAnimationMode::AnimationCustomMode);
		MeshComp->SetEnableAnimation(false);
	}
}

// アニメーション通知名に応じて、攻撃判定・回避・パリィ・死亡状態を切り替える。
void ANormalEnemy::OnAnimAction(FName ActionName)
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
		LaunchCharacter(GetActorForwardVector() * 3000.f, true, true);
	}
	else if(ActionName == "AtkEnd")
	{
		IsAttacking = false;
		AnimInst->SetIgnoreRootMotion(IsAttacking);

		AAIController* const cont = Cast<AAIController>(GetController());
		if (cont != nullptr)
		{
			cont->GetBlackboardComponent()->SetValueAsBool("IsAttackGranted", false);
		}

		GetCapsuleComponent()->SetCollisionResponseToChannel(
			ECC_GameTraceChannel5,
			ECR_Block);
	}
	else if (ActionName == "AtkStart")
	{
		IsAttacking = true;
		AnimInst->SetIgnoreRootMotion(IsAttacking);
		GetCapsuleComponent()->SetCollisionResponseToChannel(
			ECC_GameTraceChannel5,
			ECR_Ignore);
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
	else if(ActionName == "StunEnd")
	{
		IsStunned = false;
		AttributeSet->SetSp(AttributeSet->GetMaxSp());
		SetSP(AttributeSet->GetSp());
	}
	else if (ActionName == "StunStart")
	{
		IsStunned = true;

		AAIController* const cont = Cast<AAIController>(GetController());
		if (cont != nullptr)
		{
			cont->GetBlackboardComponent()->SetValueAsBool("IsAttackGranted", false);
		}
	}
	else if(ActionName == "EnableRagdoll")
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
		//GetWorldTimerManager().SetTimerForNextTick(this, &ANormalEnemy::DisableAnimationAfterRagdoll);
	}
}

// 重なりを検知した相手の種類を確認し、該当するダメージまたは取得効果を適用する。
void ANormalEnemy::OnOverlapsBegin_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!IsAlive)
	{
		return;
	}

	ECollisionChannel OtherChannel = OtherComp->GetCollisionObjectType();

	switch (OtherChannel)
	{
		case ECC_GameTraceChannel2:
			{
				FVector FXLocation = GetActorLocation();

				if (bFromSweep)
				{
					FXLocation = SweepResult.ImpactPoint;
				}
				else
				{
					FXLocation = OtherComp->GetComponentLocation();
				}

				AAIController* controller = Cast<AAIController>(GetController());
				if (controller)
				{
					controller->GetBlackboardComponent()->SetValueAsBool(FName("didNoticePlayer"), true);
				}

				if (UEffectSubsystem* EffectSubsystem = GetWorld()->GetSubsystem<UEffectSubsystem>())
				{
					EffectSubsystem->SpawnHitFX(FXLocation, FRotator::ZeroRotator);
				}
				DoHitStop(0.0001f);

				AMySword* Sword = Cast<AMySword>(OtherActor);
				if(!Sword)
				{
					GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("OtherActor is not AMySword"));
					return;
				}

				AMyPlayer* Player = Cast<AMyPlayer>(Sword->GetOwner());

				if (Player)
				{
					UE_LOG(LogTemp, Warning, TEXT("Player: %s"), *Player->GetName());
					UE_LOG(LogTemp, Warning, TEXT("AttributeSet Ptr: %p"), Player->GetAttributeSet());
					UE_LOG(LogTemp, Warning, TEXT("AtkPower: %f"), Player->GetAttributeSet() ? Player->GetAttributeSet()->GetAtkPower() : -1.f);

					if (UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent())
					{
						if (const UPlayerAttributeSet* AttrSet = ASC->GetSet<UPlayerAttributeSet>())
						{
							CalCollisionDamage(AttrSet->GetAtkPower());
						}
					}

					if (AttributeSet->GetHp() <= 0.f)
					{
						if (Player)
						{
							Player->ClearLockOn();
						}
						else
						{
							GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("player does not exist"));
						}

						// 現在は使用していない処理。
						//Death();
						return;
					}
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("OtherActor is not AMyPlayer"));
				}
			}
			break;
		case ECC_GameTraceChannel6:
			break;
	}
}

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void ANormalEnemy::OnOverlapsEnd_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

// 重なりを検知した相手の種類を確認し、該当するダメージまたは取得効果を適用する。
void ANormalEnemy::OnRWeaponOverlapsBegin_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("bIsHitPlayer is FALSE"));

	ECollisionChannel OtherChannel = OtherComp->GetCollisionObjectType();

	switch (OtherChannel)
	{
	case ECC_GameTraceChannel4:
			{

			}
			break;
	case ECC_GameTraceChannel6:
		if (AttributeSet->GetSp() <= 0.f)
		{
			Stun();
		}

		CalCollisionDamageSP(20.f);
		break;
	}
}

// 重なりを検知した相手の種類を確認し、該当するダメージまたは取得効果を適用する。
void ANormalEnemy::OnLWeaponOverlapsBegin_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("bIsHitPlayer is FALSE"));

	ECollisionChannel OtherChannel = OtherComp->GetCollisionObjectType();

	switch (OtherChannel)
	{
	case ECC_GameTraceChannel4:
	{
		// 現在は使用していない処理。
		//if (AMyPlayer* Player = Cast<AMyPlayer>(OtherActor))
		//{
		//	if (bIsHitPlayer == true)
		//	{
		//		return;
		//	}

		// 現在は使用していない処理。
		//	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("L HIT"));
		//	if (UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent())
		//	{
		//		if (const UPlayerAttributeSet* AttrSet = ASC->GetSet<UPlayerAttributeSet>())
		//		{
		//			bIsHitPlayer = true;
		//			Player->CalCollisionDamage(AttrSet->GetAtkPower());
		//		}
		//	}
		//}
	}
	break;
	case ECC_GameTraceChannel6:
		if (AttributeSet->GetSp() <= 0.f)
		{
			Stun();
		}

		CalCollisionDamageSP(20.f);
		break;
	}
}

// 左武器の衝突判定を有効にし、直前のソケット位置を記録する。
void ANormalEnemy::StartTraceAttackL()
{
	IsTraceAttackActiveL = true;
	HitActors.Reset();

	if (GetMesh())
	{
		PrevStart = GetMesh()->GetSocketLocation(TraceStartSocketL);
		PrevEnd = GetMesh()->GetSocketLocation(TraceEndSocketL);
	}
}

// 左武器の衝突判定を終了し、命中対象の記録を整理する。
void ANormalEnemy::EndTraceAttackL()
{
	IsTraceAttackActiveL = false;
	HitActors.Reset();
}

// 左武器の前回・現在位置をスイープし、攻撃の衝突を検出する。
void ANormalEnemy::PerformWeaponTraceL()
{
	if (!GetMesh())
	{
		return;
	}

	const FVector CurrentStart = GetMesh()->GetSocketLocation(TraceStartSocketL);
	const FVector CurrentEnd = GetMesh()->GetSocketLocation(TraceEndSocketL);

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


	TArray<FHitResult> EndHitResults;
	UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(),
		PrevEnd,
		CurrentEnd,
		TraceRadius,
		UEngineTypes::ConvertToTraceType(TraceChannel),
		false,
		ActorsToIgnore,
		bDrawDebugTrace ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		EndHitResults,
		true
	);

	HitResults.Append(EndHitResults);

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
				return;
			}
			if (Player->GetIsEvading())
			{
				return;
			}

			if (bIsHitPlayer == true)
			{
				return;
			}

			bIsHitPlayer = true;
			Player->CalCollisionDamage(AttributeSet->GetAtkPower(), this);
		}
	}

	PrevStart = CurrentStart;
	PrevEnd = CurrentEnd;
}

// 右武器の衝突判定を有効にし、直前のソケット位置を記録する。
void ANormalEnemy::StartTraceAttackR()
{
	IsTraceAttackActiveR = true;
	HitActors.Reset();

	if (GetMesh())
	{
		PrevStart = GetMesh()->GetSocketLocation(TraceStartSocketR);
		PrevEnd = GetMesh()->GetSocketLocation(TraceEndSocketR);
	}
}

// 右武器の衝突判定を終了し、命中対象の記録を整理する。
void ANormalEnemy::EndTraceAttackR()
{
	IsTraceAttackActiveR = false;
	HitActors.Reset();
}

// 右武器の前回・現在位置をスイープし、攻撃の衝突を検出する。
void ANormalEnemy::PerformWeaponTraceR()
{
	if (!GetMesh())
	{
		return;
	}

	const FVector CurrentStart = GetMesh()->GetSocketLocation(TraceStartSocketR);
	const FVector CurrentEnd = GetMesh()->GetSocketLocation(TraceEndSocketR);

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


	TArray<FHitResult> EndHitResults;
	UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(),
		PrevEnd,
		CurrentEnd,
		TraceRadius,
		UEngineTypes::ConvertToTraceType(TraceChannel),
		false,
		ActorsToIgnore,
		bDrawDebugTrace ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		EndHitResults,
		true
	);

	HitResults.Append(EndHitResults);

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor)
		{
			continue;
		}

		if (AMyPlayer* Player = Cast<AMyPlayer>(HitActor))
		{
			if(Player->GetIsParrying())
			{
				Player->SetParrying(Hit.ImpactPoint);


				if (AttributeSet->GetSp() <= 0.f)
				{
					Stun();
				}

				// 現在は使用していない処理。
				//CalCollisionDamageSP(20.f);
				EndTraceAttackR();
				return;
			}
			if (Player->GetIsEvading())
			{
				return;
			}

			if (bIsHitPlayer == true)
			{
				return;
			}

			bIsHitPlayer = true;
			Player->CalCollisionDamage(AttributeSet->GetAtkPower(), this);
		}
	}

	PrevStart = CurrentStart;
	PrevEnd = CurrentEnd;
}
