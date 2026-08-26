// 敵固有の行動と戦闘を処理する。

#include "Enemy/WeakEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "../../MyAnimInstancePj26.h"
#include <Kismet/GameplayStatics.h>
#include "Subsystem/EffectSubsystem.h"
#include "Subsystem/GameAudioSubsystem.h"
#include <AIController.h>
#include "EnemyAI/WeakEnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Data/EnemyInfo.h"
#include "BrainComponent.h"
#include "../../MyPlayer.h"
#include "AttributeSet/EnemyAttributeSet.h"
#include "AttributeSet/PlayerAttributeSet.h"
#include "../../Public/MySword.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
AWeakEnemy::AWeakEnemy()
{

	PrimaryActorTick.bCanEverTick = true;

	AtkRange = 150.f;


	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	Weapon = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("Muzzle_02"));
	Weapon->SetRelativeLocation(FVector::ZeroVector);

	AbilitySystemComp = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	AttributeSet = CreateDefaultSubobject<UEnemyAttributeSet>(TEXT("AttributeSet"));

	HpWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HpWidgetComponent"));
	HpWidgetComponent->SetupAttachment(GetRootComponent());
	HpWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HpWidgetComponent->SetDrawSize(FVector2D(180.f, 80.f));
	HpWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void AWeakEnemy::BeginPlay()
{
	Super::BeginPlay();

	AnimInst = Cast<UMyAnimInstancePj26>(GetMesh()->GetAnimInstance());

	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Weapon->SetGenerateOverlapEvents(false);

	if (EnemyInfoTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyInfoTable: %s"), *EnemyInfoTable->GetName());

		static const FString ContextString(TEXT("Enemy Info Context"));
		FName RowName = TEXT("WeakEnemy");

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

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void AWeakEnemy::ToggleUI()
{
}

// 生存状態を解除し、移動・衝突・戦闘を停止した後に死亡演出を開始する。
void AWeakEnemy::Death()
{
	IsAlive = false;
	IsStunned = false;
	IsTraceAttackActive = false;

	RemoveAllDebuffs();

	HpWidgetComponent->SetVisibility(false);
	HpWidgetComponent->Deactivate();

	GetCapsuleComponent()->SetCollisionResponseToChannel(
		ECC_GameTraceChannel4,
		ECR_Ignore);

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
void AWeakEnemy::CalCollisionDamage(AActor* OtherActor)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("NOT HERE")));
}

// 衝突相手とダメージ量を確認し、プレイヤーへ通常の衝突ダメージを適用する。
void AWeakEnemy::CalCollisionDamage(float DamageAmount)
{
	UEffectSubsystem* EffectSubsystem = GetWorld()->GetSubsystem<UEffectSubsystem>();
	if (EffectSubsystem)
	{
		EffectSubsystem->SpawnHitFX(GetActorLocation(), FRotator::ZeroRotator);
	}

	AttributeSet->SetHp(FMath::Clamp(AttributeSet->GetHp() - DamageAmount, 0.f, AttributeSet->GetMaxHp()));
	SetHP(AttributeSet->GetHp() - DamageAmount);

	if (AttributeSet->GetHp() <= 0.f)
	{
		Death();
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

// 衝突相手を確認し、プレイヤーへ属性またはスタミナに関するダメージを適用する。
void AWeakEnemy::CalCollisionDamageSP(float DamageAmount)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("DamageAmount: %f"), DamageAmount));
	AttributeSet->SetSp(FMath::Clamp(AttributeSet->GetSp() - DamageAmount, 0.f, AttributeSet->GetMaxSp()));
	SetSP(AttributeSet->GetSp() - DamageAmount);
}

// 被弾状態へ切り替え、攻撃中断・アニメーション・関連演出を処理する。
void AWeakEnemy::Damaged()
{
	EndTraceAttack();

	if (IsAlive == false)
	{
		return;
	}

	if (IsStunned)
	{
		return;
	}

	AnimInst->SetIgnoreRootMotion(false);
	AnimInst->Montage_Play(DmgAnimMontage);
}

// 気絶状態へ切り替え、現在の行動を停止して気絶演出を開始する。
void AWeakEnemy::Stun()
{
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Weapon->SetGenerateOverlapEvents(false);

	IsTraceAttackActive = false;

	AnimInst->StopAllMontages(0.0f);
	AnimInst->Montage_Play(StunAnimMontage);
}

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void AWeakEnemy::UpdateHealth(float changedHP)
{
}

// 構成要素の生成完了後に、相互参照とイベントを接続する。
void AWeakEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	GetCapsuleComponent()->OnComponentBeginOverlap.__Internal_AddDynamic(this, &AWeakEnemy::OnOverlapsBegin, FName("OnOverlapsBegin"));
	GetCapsuleComponent()->OnComponentEndOverlap.__Internal_AddDynamic(this, &AWeakEnemy::OnOverlapsEnd, FName("OnOverlapsEnd"));

	Weapon->OnComponentBeginOverlap.__Internal_AddDynamic(this, &AWeakEnemy::OnWeaponOverlapsBegin, FName("OnWeaponOverlapsBegin"));

}

// ラグドール移行後にアニメーション更新を停止し、物理姿勢だけを維持する。
void AWeakEnemy::DisableAnimationAfterRagdoll()
{
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetAnimationMode(EAnimationMode::AnimationCustomMode);
		MeshComp->SetEnableAnimation(false);
	}
}

// アニメーション通知名に応じて、攻撃判定・回避・パリィ・死亡状態を切り替える。
void AWeakEnemy::OnAnimAction(FName ActionName)
{
	if (ActionName == "AtkEnd")
	{
		IsAttacking = false;

		AAIController* const cont = Cast<AAIController>(GetController());
		if (cont != nullptr)
		{
			cont->GetBlackboardComponent()->SetValueAsBool("IsAttackGranted", false);
		}
	}
	else if (ActionName == "AtkStart")
	{
		IsAttacking = true;
	}
	else if (ActionName == "AtkCollEnable")
	{
		// 現在は使用していない処理。
		//Weapon->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		//Weapon->SetGenerateOverlapEvents(true);
		StartTraceAttack();
	}
	else if (ActionName == "AtkCollDisable")
	{
		// 現在は使用していない処理。
		//Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Weapon->SetGenerateOverlapEvents(false);
		EndTraceAttack();
		bIsHitPlayer = false;
	}
	else if (ActionName == "StunEnd")
	{
		IsStunned = false;
		AttributeSet->SetSp(AttributeSet->GetMaxSp());
		SetSP(AttributeSet->GetSp());
	}
	else if (ActionName == "StunStart")
	{
		IsAttacking = false;
		IsStunned = true;
	}
	else if (ActionName == "DamageStart")
	{
		IsAttacking = false;
		CanTakeDamage = false;
	}
	else if (ActionName == "DamageEnd")
	{

		IsAttacking = false;
		CanTakeDamage = true;
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
		//GetWorldTimerManager().SetTimerForNextTick(this, &AWeakEnemy::DisableAnimationAfterRagdoll);
	}

}

// 攻撃開始時に武器の衝突判定を有効にし、直前のソケット位置を記録する。
void AWeakEnemy::StartTraceAttack()
{
	IsTraceAttackActive = true;
	HitActors.Reset();

	if (GetMesh())
	{
		PrevStart = GetMesh()->GetSocketLocation(TraceStartSocket);
		PrevEnd = GetMesh()->GetSocketLocation(TraceEndSocket);
	}
}

// 攻撃終了時に武器の衝突判定を停止し、命中対象の記録を初期化する。
void AWeakEnemy::EndTraceAttack()
{
	IsTraceAttackActive = false;
	HitActors.Reset();
}

// 前回と現在の武器位置の間をスイープし、高速な攻撃の衝突を検出する。
void AWeakEnemy::PerformWeaponTrace()
{
	if (!GetMesh())
	{
		return;
	}


	if (bIsHitPlayer == true)
	{
		return;
	}

	const FVector CurrentStart = GetMesh()->GetSocketLocation(TraceStartSocket);
	const FVector CurrentEnd = GetMesh()->GetSocketLocation(TraceEndSocket);

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

				CalCollisionDamageSP(20.f);
				EndTraceAttack();
				return;
			}
			if (Player->GetIsEvading())
			{
				return;
			}

			bIsHitPlayer = true;
			Player->CalCollisionDamage(AttributeSet->GetAtkPower(), this);
			return;
		}
	}

	PrevStart = CurrentStart;
	PrevEnd = CurrentEnd;

}


// 対象とプレイヤーの時間倍率を下げ、タイマーで通常速度への復帰を予約する。
void AWeakEnemy::DoHitStop(float Duration)
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0011f);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,
		FTimerDelegate::CreateUObject(this, &AWeakEnemy::ResetTimeDilation, Duration),
		Duration, false);
}

// 一時的に変更した時間倍率を通常の値へ戻す。
void AWeakEnemy::ResetTimeDilation(float Duration)
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
}

// 体力割合を取得し、呼び出し元へ返す。
float AWeakEnemy::GetHpPercent() const
{
	return AttributeSet->GetMaxHp() > 0.f ? AttributeSet->GetHp() / AttributeSet->GetMaxHp() : 0.f;
}

// スタミナ割合を取得し、呼び出し元へ返す。
float AWeakEnemy::GetSpPercent() const
{
	return AttributeSet->GetMaxSp() > 0.f ? AttributeSet->GetSp() / AttributeSet->GetMaxSp() : 0.f;
}

// 体力を指定された値へ更新する。
void AWeakEnemy::SetHP(float NewHP)
{
	if (UEnemyHPWidget* EnemyHPWidget = Cast<UEnemyHPWidget>(HpWidgetComponent->GetUserWidgetObject()))
	{
		EnemyHPWidget->UpdateHp();
	}
}

// スタミナを指定された値へ更新する。
void AWeakEnemy::SetSP(float NewSP)
{
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
void AWeakEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AnimInst->IsInAir = GetCharacterMovement()->IsFalling();

	FVector Vel = GetVelocity();
	Vel.Z = 0.f;
	AnimInst->Speed = Vel.Size();

	if (IsTraceAttackActive)
	{
		PerformWeaponTrace();
	}
}


// 移動・視点・戦闘に使用する入力と処理関数を関連付ける。
void AWeakEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// プレイヤーに関するイベントを受け取り、関連状態を更新する。
void AWeakEnemy::OnFollowPlayer(bool _isPlayerInSight)
{
	if (_isPlayerInSight == true)
	{
		GetCharacterMovement()->MaxWalkSpeed = 250.f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 50.f;
	}

	isPlayerInSight = _isPlayerInSight;
}

// 現在の戦闘状態と入力条件を確認し、対応する攻撃処理を実行する。
void AWeakEnemy::MeleeAttack()
{
	if(IsStunned)
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance || !AtkMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance or AtkMontage invalid"));
		return;
	}

	float Result = AnimInstance->Montage_Play(AtkMontage);
}

// 重なりを検知した相手の種類を確認し、該当するダメージまたは取得効果を適用する。
void AWeakEnemy::OnOverlapsBegin_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
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
		CalCollisionDamage(OtherActor);
	}
	break;
	case ECC_GameTraceChannel6:
		Stun();
		break;
	}
}

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void AWeakEnemy::OnOverlapsEnd_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

// 重なりを検知した相手の種類を確認し、該当するダメージまたは取得効果を適用する。
void AWeakEnemy::OnWeaponOverlapsBegin_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ECollisionChannel OtherChannel = OtherComp->GetCollisionObjectType();

	// 現在は使用していない処理。
	//switch (OtherChannel)
	//{
	//case ECC_GameTraceChannel6:
	//	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("Stun"));
	//	Stun();
	//	break;
	//}
}
