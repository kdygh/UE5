// 敵固有の行動と戦闘を処理する。

#include "enemy/DashEnemy.h"
#include "EnemyAI/DashEnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "../../MyAnimInstancePj26.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include "EnemyAI/DashEnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Subsystem/EffectSubsystem.h"
#include "Subsystem/GameAudioSubsystem.h"
#include "Data/EnemyInfo.h"
#include "BrainComponent.h"
#include "../../MyPlayer.h"
#include "AttributeSet/EnemyAttributeSet.h"
#include "AttributeSet/PlayerAttributeSet.h"
#include "../../Public/MySword.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
ADashEnemy::ADashEnemy()
{

	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	IsPredictionOn = false;
	IsStunned = false;

	DashCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("DashCollision"));
	DashCollision->SetupAttachment(GetMesh(), TEXT("pelvis"));
	DashCollision->SetRelativeLocation(FVector::ZeroVector);

	SplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Comp"));

	AbilitySystemComp = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	AttributeSet = CreateDefaultSubobject<UEnemyAttributeSet>(TEXT("AttributeSet"));

	UE_LOG(LogTemp, Warning, TEXT("AbilitySystemComp Ptr: %p"), Cast<UAbilitySystemComponent>(AbilitySystemComp));
	UE_LOG(LogTemp, Warning, TEXT("Member AttributeSet Ptr: %p"), Cast<UEnemyAttributeSet>(AttributeSet));

	HpWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HpWidgetComponent"));
	HpWidgetComponent->SetupAttachment(GetRootComponent());
	HpWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HpWidgetComponent->SetDrawSize(FVector2D(180.f, 80.f));
	HpWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void ADashEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComp)
	{
		AbilitySystemComp->InitAbilityActorInfo(this, this);

		const UEnemyAttributeSet* ASCSet = AbilitySystemComp->GetSet<UEnemyAttributeSet>();
		UE_LOG(LogTemp, Warning, TEXT("ASC AttributeSet Ptr: %p"), ASCSet);
	}

	AnimInst = Cast<UMyAnimInstancePj26>(GetMesh()->GetAnimInstance());

	DashCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DashCollision->SetGenerateOverlapEvents(false);

	if (EnemyInfoTable)
	{
		static const FString ContextString(TEXT("Enemy Info Context"));
		FName RowName = TEXT("DashEnemy");

		FEnemyInfo* EnemyInfo = EnemyInfoTable->FindRow<FEnemyInfo>(RowName, ContextString);

		if (EnemyInfo)
		{
			MaxHP = EnemyInfo->HP;
			AtkPower = EnemyInfo->AtkPower;
			CurrHP = MaxHP;

			if(AbilitySystemComp)
			{
				AbilitySystemComp->InitAbilityActorInfo(this, this);
			}

			if(AttributeSet == nullptr)
			{
				return;
			}

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
		UE_LOG(LogTemp, Warning, TEXT("EnemyHPWidget found! Class: %s"), *GetClass()->GetName());
		EnemyHPWidget->SetOwnerEnemy(this);
	}
}

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void ADashEnemy::ToggleUI()
{
}

// 生存状態を解除し、移動・衝突・戦闘を停止した後に死亡演出を開始する。
void ADashEnemy::Death()
{
	IsAlive = false;
	IsStunned = false;

	StopSearching();
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
		TimerHandle, [this]() {Destroy(); }, 2.0f, false
	);
}

// 衝突相手とダメージ量を確認し、プレイヤーへ通常の衝突ダメージを適用する。
void ADashEnemy::CalCollisionDamage(AActor* OtherActor)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("NOT HERE")));
}

// 衝突相手とダメージ量を確認し、プレイヤーへ通常の衝突ダメージを適用する。
void ADashEnemy::CalCollisionDamage(float DamageAmount)
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
void ADashEnemy::CalCollisionDamageSP(float DamageAmount)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("DamageAmount: %f"), DamageAmount));
	AttributeSet->SetSp(FMath::Clamp(AttributeSet->GetSp() - DamageAmount, 0.f, AttributeSet->GetMaxSp()));
	SetSP(AttributeSet->GetSp() - DamageAmount);
}

// 被弾状態へ切り替え、攻撃中断・アニメーション・関連演出を処理する。
void ADashEnemy::Damaged()
{
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

	if (AttributeSet->GetSp() <= 0.f)
	{
		EndTraceAttack();
	}
}

// 気絶状態へ切り替え、現在の行動を停止して気絶演出を開始する。
void ADashEnemy::Stun()
{
	IsStunned = true;
	DashCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DashCollision->SetGenerateOverlapEvents(false);

	AnimInst->Montage_Play(m_animMontage);

	IsAttacking = false;
	AnimInst->IsAttacking = false;
	AnimInst->SetIgnoreRootMotion(false);
}

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void ADashEnemy::UpdateHealth(float changedHP)
{
}

// 構成要素の生成完了後に、相互参照とイベントを接続する。
void ADashEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	DashCollision->OnComponentBeginOverlap.__Internal_AddDynamic(this, &ADashEnemy::OnOverlapsBegin, FName("OnOverlapsBegin"));
}

// 対象とプレイヤーの時間倍率を下げ、タイマーで通常速度への復帰を予約する。
void ADashEnemy::DoHitStop(float Duration)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("DoHitStop"));
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0015f);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,
		FTimerDelegate::CreateUObject(this, &ADashEnemy::ResetTimeDilation, Duration),
		Duration, false);
}

// 一時的に変更した時間倍率を通常の値へ戻す。
void ADashEnemy::ResetTimeDilation(float Duration)
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
}

// 命中を指定された値へ更新する。
void ADashEnemy::SetIsHitByObject(bool _isHit)
{
	bIsHitByObject = _isHit;
}

// 体力割合を取得し、呼び出し元へ返す。
float ADashEnemy::GetHpPercent() const
{
	return AttributeSet->GetMaxHp() > 0.f ? AttributeSet->GetHp() / AttributeSet->GetMaxHp() : 0.f;
}

// スタミナ割合を取得し、呼び出し元へ返す。
float ADashEnemy::GetSpPercent() const
{
	return AttributeSet->GetMaxSp() > 0.f ? AttributeSet->GetSp() / AttributeSet->GetMaxSp() : 0.f;
}

// 体力を指定された値へ更新する。
void ADashEnemy::SetHP(float NewHP)
{
	if (UEnemyHPWidget* EnemyHPWidget = Cast<UEnemyHPWidget>(HpWidgetComponent->GetUserWidgetObject()))
	{
		EnemyHPWidget->UpdateHp();
	}
}

// スタミナを指定された値へ更新する。
void ADashEnemy::SetSP(float NewSP)
{
	if (UEnemyHPWidget* EnemyHPWidget = Cast<UEnemyHPWidget>(HpWidgetComponent->GetUserWidgetObject()))
	{
		EnemyHPWidget->UpdateSp();
	}
}


// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void ADashEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsPredictionOn == true)
		DrawPredictionSpline();

	if (AnimInst->IsAttacking == true)
	{
		AddMovementInput(GetActorForwardVector(), 0.5f, false);
	}

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
void ADashEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void ADashEnemy::OnFollowPlayer(bool _isPlayerInSight)
{
	// 現在は使用していない処理。
	//SearchPlayerPos();
}

// 現在の戦闘状態と入力条件を確認し、対応する攻撃処理を実行する。
void ADashEnemy::MeleeAttack()
{
	StopSearching();

	if (AMyPlayer* Player = Cast<AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		if (Player->GetIsAlive() == false)
		{
			return;
		}
	}

	DashCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DashCollision->SetGenerateOverlapEvents(true);

	IsAttacking = true;
	IsTraceAttackActive = true;
	IsPredictionOn = false;

	AnimInst->IsPreparingAtk = false;
	AnimInst->IsAttacking = true;
	GetCharacterMovement()->MaxWalkSpeed = 2000.0f;
	AnimInst->SetIgnoreRootMotion(true);
}

// ラグドール移行後にアニメーション更新を停止し、物理姿勢だけを維持する。
void ADashEnemy::DisableAnimationAfterRagdoll()
{
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetAnimationMode(EAnimationMode::AnimationCustomMode);
		MeshComp->SetEnableAnimation(false);
	}
}

// アニメーション通知名に応じて、攻撃判定・回避・パリィ・死亡状態を切り替える。
void ADashEnemy::OnAnimAction(FName ActionName)
{
	if (ActionName == "EnableRagdoll")
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
		//GetWorldTimerManager().SetTimerForNextTick(this, &ADashEnemy::DisableAnimationAfterRagdoll);
	}
	else if (ActionName == "DashStart")
	{
		if (AAIController* AICon = Cast<AAIController>(GetController()))
		{
			AICon->GetBlackboardComponent()->SetValueAsBool(TEXT("isAbleToAtk"), true);
		}
	}
	else if (ActionName == "StartCollided" || ActionName == "PrepareDash")
	{
		if (AAIController* AICon = Cast<AAIController>(GetController()))
		{
			AICon->GetBlackboardComponent()->SetValueAsBool(TEXT("isAbleToAtk"), false);
		}
	}
	else if (ActionName == "EndCollided")
	{
		IsStunned = false;
		SetIsHitByObject(false);
	}
	else if (ActionName == "DamageStart")
	{
		SetIsHitByObject(true);
	}
	else if (ActionName == "DamageEnd")
	{
		SetIsHitByObject(false);
	}
}

// プレイヤーの現在位置と移動方向を使用し、攻撃目標地点を計算する。
void ADashEnemy::SearchPlayerPos()
{
	UE_LOG(LogTemp, Log, TEXT("ADashEnemy::SearchPlayerPos"));

	UE_LOG(LogTemp, Log, TEXT("Player Location: %s"), *GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation().ToString());
	AnimInst->IsPreparingAtk = true;

	CreatePredictionSpline();
}

// 目標位置の検索を終了し、関連するタイマーと状態を初期化する。
void ADashEnemy::StopSearching()
{
	UE_LOG(LogTemp, Log, TEXT("ADashEnemy::StopSearching"));

	if (ChargeWarningActor)
	{
		ChargeWarningActor->Destroy();
		ChargeWarningActor = nullptr;
	}

	AnimInst->IsPreparingAtk = false;
}

// 予測軌道の計算に使用するスプラインと表示用構成要素を生成する。
void ADashEnemy::CreatePredictionSpline()
{
	IsPredictionOn = true;

	ChargeWarningActor = GetWorld()->SpawnActor<AChargeWarningActor>(
		ChargeWarningClass,
		GetActorLocation(),
		FRotator::ZeroRotator
	);

	AMyPlayer* Player = Cast<AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());

	ChargeWarningActor->InitializeWarning(this, Player, 10.0f);
}

// 計算した投射経路に沿ってスプラインメッシュを配置し、予測線を表示する。
void ADashEnemy::DrawPredictionSpline()
{
	FVector startLoc = GetActorLocation();
	FVector targetLoc = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	targetLoc = FVector(targetLoc.X, targetLoc.Y, startLoc.Z);
	float arcVal = 0.99f;

	FRotator rot = GetActorRotation();
	FRotator targetRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(),
		targetLoc);
	FRotator lerpRot = UKismetMathLibrary::RLerp(rot, targetRot, 0.5f, true);

	SetActorRotation(FRotator(0.f, lerpRot.Yaw, 0.f));
}

// 攻撃開始時に武器の衝突判定を有効にし、直前のソケット位置を記録する。
void ADashEnemy::StartTraceAttack()
{
	IsTraceAttackActive = true;
	HitActors.Reset();

	if (GetMesh())
	{
		PrevStart = GetMesh()->GetSocketLocation(TraceStartSocket);
	}
}

// 攻撃終了時に武器の衝突判定を停止し、命中対象の記録を初期化する。
void ADashEnemy::EndTraceAttack()
{
	UGameAudioSubsystem* AudioSubsystem = GetGameInstance()->GetSubsystem<UGameAudioSubsystem>();
	if (AudioSubsystem)
	{
		AudioSubsystem->PlayHitSound(GetActorLocation());
	}

	SetIsHitByObject(true);
	Stun();
	IsTraceAttackActive = false;
	HitActors.Reset();
	bIsHitPlayer = false;
}

// 重なりを検知した相手の種類を確認し、該当するダメージまたは取得効果を適用する。
void ADashEnemy::OnOverlapsBegin_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	EndTraceAttack();
}

// 前回と現在の武器位置の間をスイープし、高速な攻撃の衝突を検出する。
void ADashEnemy::PerformWeaponTrace()
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

				EndTraceAttack();
				return;
			}

			bIsHitPlayer = true;
			Player->CalCollisionDamage(AttributeSet->GetAtkPower(), this);
			EndTraceAttack();
			return;
		}
		else if (AEnemyOrigin* Enemy = Cast<AEnemyOrigin>(HitActor))
		{
			Enemy->CalCollisionDamage(AttributeSet->GetAtkPower());
			EndTraceAttack();
			return;
		}
	}

	PrevStart = CurrentStart;
}
