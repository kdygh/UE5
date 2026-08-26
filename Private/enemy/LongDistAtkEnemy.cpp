// 敵固有の行動と戦闘を処理する。

#include "enemy/LongDistAtkEnemy.h"
#include "Components/SplineMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "LongdistAtkPredictLine.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "LongDistAtkPredictPoint.h"
#include "../../MyAnimInstancePj26.h"
#include "../MyPlayer.h"
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "Subsystem/EffectSubsystem.h"
#include "Data/EnemyInfo.h"
#include "Subsystem/EffectActorSubsystem.h"
#include "Subsystem/CombatFeedbackSubsystem.h"
#include "Subsystem/GameAudioSubsystem.h"
#include "Objects/EnemyExplosionActor.h"
#include "EnemyAI/LongDistAtkEnemyAIController.h"
#include "BrainComponent.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
ALongDistAtkEnemy::ALongDistAtkEnemy()
{

	PrimaryActorTick.bCanEverTick = true;


	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	ExplosionCollider = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollider"));
	ExplosionCollider->SetupAttachment(RootComponent);

	SplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Comp"));
	SplineComp->SetupAttachment(RootComponent);

	ProjectileComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileComp->MaxSpeed = 2500.f;
	ProjectileComp->bShouldBounce = false;
	ProjectileComp->bRotationFollowsVelocity = true;
	ProjectileComp->Bounciness = 0.0f;
	ProjectileComp->Friction = 0.0f;

	AbilitySystemComp = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	AttributeSet = CreateDefaultSubobject<UEnemyAttributeSet>(TEXT("AttributeSet"));

	HpWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HpWidgetComponent"));
	HpWidgetComponent->SetupAttachment(GetRootComponent());
	HpWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HpWidgetComponent->SetDrawSize(FVector2D(180.f, 80.f));
	HpWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	HpWidgetComponent->SetVisibility(false);

	bIsPredictionOn = false;

	bIsBounced = false;
}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void ALongDistAtkEnemy::BeginPlay()
{
	Super::BeginPlay();
	AnimInst = Cast<UMyAnimInstancePj26>(GetMesh()->GetAnimInstance());

	ExplosionCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ExplosionCollider->SetGenerateOverlapEvents(true);

	AAIController* controller = Cast<AAIController>(GetController());
	if (controller == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ALongDistAtkEnemy::BeginPlay() - Controller is nullptr"));
	}

	if (EnemyInfoTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyInfoTable: %s"), *EnemyInfoTable->GetName());

		static const FString ContextString(TEXT("Enemy Info Context"));
		FName RowName = TEXT("LongDistAtkEnemy");

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
}

// 構成要素の生成完了後に、相互参照とイベントを接続する。
void ALongDistAtkEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetCapsuleComponent()->OnComponentBeginOverlap.__Internal_AddDynamic(this, &ALongDistAtkEnemy::OnOverlapsBegin, FName("OnOverlapsBegin"));

	if (ProjectileComp)
	{
		ProjectileComp->OnProjectileBounce.__Internal_AddDynamic(this, &ALongDistAtkEnemy::KeepSettingVelocity, FName("KeepSettingVelocity"));
	}

	if (ExplosionCollider)
	{
		ExplosionCollider->OnComponentBeginOverlap.__Internal_AddDynamic(this, &ALongDistAtkEnemy::OnExCollOverlapsBegin, FName("OnExCollOverlapsBegin"));
	}

}

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void ALongDistAtkEnemy::ToggleUI()
{
}

// 生存状態を解除し、移動・衝突・戦闘を停止した後に死亡演出を開始する。
void ALongDistAtkEnemy::Death()
{
	IsAlive = false;

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

	if (UEffectSubsystem* EffectSubsystem = GetWorld()->GetSubsystem<UEffectSubsystem>())
	{
		EffectSubsystem->SpawnExplosionFX(GetActorLocation(), FRotator::ZeroRotator);
	}

	if (UGameAudioSubsystem* AudioSubsystem =
		GetGameInstance()->GetSubsystem<UGameAudioSubsystem>())
	{
		AudioSubsystem->PlayExplosionSound(GetActorLocation());
	}

	if(UCombatFeedbackSubsystem* CombatFeedbackSubsystem = GetWorld()->GetSubsystem<UCombatFeedbackSubsystem>())
	{
		CombatFeedbackSubsystem->PlayCameraShake(ExplosionCameraShakeClass, 10.0f);
	}

	if (UEffectActorSubsystem* Pool = GetWorld()->GetSubsystem<UEffectActorSubsystem>())
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(GetActorLocation());

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Spawning Explosion Actor from Pool!"));
		Pool->AcquireActor(ExplosionActorClass, SpawnTransform, GetActorForwardVector());
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("EffectActorSubsystem not found!"));
	}

	DestroyPredictionSpline();

	bIsBounced = true;
	Destroy();
}

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void ALongDistAtkEnemy::CalCollisionDamage(AActor* OtherActor)
{
}

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void ALongDistAtkEnemy::UpdateHealth(float changedHP)
{
}

// 体力割合を取得し、呼び出し元へ返す。
float ALongDistAtkEnemy::GetHpPercent() const
{
	return 0.0f;
}

// 体力を指定された値へ更新する。
void ALongDistAtkEnemy::SetHP(float NewHP)
{
	if (UEnemyHPWidget* EnemyHPWidget = Cast<UEnemyHPWidget>(HpWidgetComponent->GetUserWidgetObject()))
	{
		EnemyHPWidget->UpdateHp();
	}
}


// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void ALongDistAtkEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsPredictionOn)
	{
		DrawPredictionSpline();
	}
}


// 移動・視点・戦闘に使用する入力と処理関数を関連付ける。
void ALongDistAtkEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// 現在の戦闘状態と入力条件を確認し、対応する攻撃処理を実行する。
void ALongDistAtkEnemy::MeleeAttack()
{
	if (AMyPlayer* Player = Cast<AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		if (Player->GetIsAlive() == false)
		{
			DestroyPredictionSpline();
			return;
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ALongDistAtkEnemy::MeleeAttack"));

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);

	ThrowYourself(GetController()->GetPawn()->GetTransform(), GetActorLocation(), AtkVelocity);
}

// プレイヤーの現在位置と移動方向を使用し、攻撃目標地点を計算する。
void ALongDistAtkEnemy::SearchPlayerPos()
{
	UE_LOG(LogTemp, Log, TEXT("ALongDistAtkEnemy::SearchPlayerPos"));

	UE_LOG(LogTemp, Log, TEXT("Player Location: %s"), *GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation().ToString());
	// 現在は使用していない処理。
	//AnimInst->IsPreparingAtk = true;

	CreatePredictionSpline();
}

// 計算した速度を適用し、敵自身を目標地点の方向へ発射する。
void ALongDistAtkEnemy::ThrowYourself(const FTransform& _transform, const FVector& _pos, const FVector& _velocity)
{
	DestroyPredictionSpline();

	SetActorLocation(_pos);

	ProjectileComp->ProjectileGravityScale = 1.0f;
	ProjectileComp->StopMovementImmediately();
	ProjectileComp->Velocity = _velocity;
	ProjectileComp->UpdateComponentVelocity();

	UE_LOG(LogTemp, Warning, TEXT("Actual Start: %s"), *_pos.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Velocity: %s"), *_velocity.ToString());
	UE_LOG(LogTemp, Warning, TEXT("WorldGravity: %f"), GetWorld()->GetGravityZ());
	UE_LOG(LogTemp, Warning, TEXT("ProjectileGravityScale: %f"), ProjectileComp->ProjectileGravityScale);
}

// 予測軌道の計算に使用するスプラインと表示用構成要素を生成する。
void ALongDistAtkEnemy::CreatePredictionSpline()
{
	bIsPredictionOn = true;

	bool isspline = SplineComp != nullptr;

	FActorSpawnParameters spawnParams;
	FRotator rot;
	FVector spawnLocation = FVector::ZeroVector;

	PredictionPoint = GetWorld()->SpawnActor<ALongDistAtkPredictPoint>(ALongDistAtkPredictPoint::StaticClass(),
		spawnLocation, rot, spawnParams);

	PredictionPoint->SetActorHiddenInGame(true);
}

// 計算した投射経路に沿ってスプラインメッシュを配置し、予測線を表示する。
void ALongDistAtkEnemy::DrawPredictionSpline()
{
	if (!SplineComp || !ProjectileComp)
		return;

	FVector StartLoc = GetActorLocation();
	FVector TargetLoc = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	float ArcVal = 0.5f;

	FRotator rot = GetActorRotation();
	FRotator targetRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(),
		TargetLoc);
	FRotator lerpRot = UKismetMathLibrary::RLerp(rot, targetRot, 0.5f, true);

	SetActorRotation(FRotator(0.f, lerpRot.Yaw, 0.f));

	FVector OutVelocity = FVector::ZeroVector;

	const float ActualGravityZ = GetWorld()->GetGravityZ() * ProjectileComp->ProjectileGravityScale;

	if (UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		this,
		OutVelocity,
		StartLoc,
		TargetLoc,
		ActualGravityZ,
		ArcVal))
	{
		FPredictProjectilePathParams PredictParams(0.1f, StartLoc, OutVelocity, 5.f);
		PredictParams.DrawDebugType = EDrawDebugTrace::None;
		PredictParams.OverrideGravityZ = ActualGravityZ;
		PredictParams.ActorsToIgnore.Add(this);
		PredictParams.bTraceWithCollision = true;

		FPredictProjectilePathResult Result;

		bool bPathExists = UGameplayStatics::PredictProjectilePath(this, PredictParams, Result);

		if (bPathExists)
		{
			FVector HitPos = Result.HitResult.ImpactPoint;

			const int32 RequiredCount = Result.PathData.Num();

			for (int32 Index = 0; Index < RequiredCount; ++Index)
			{
				ALongdistAtkPredictLine* Line =
					GetOrCreatePredictLine(Index);

				if (!IsValid(Line))
				{
					continue;
				}

				FVector Direction = Result.PathData[Index].Velocity;

				if (Direction.IsNearlyZero() &&
					Result.PathData.IsValidIndex(Index + 1))
				{
					Direction =
						Result.PathData[Index + 1].Location -
						Result.PathData[Index].Location;
				}

				Line->SetActorLocationAndRotation(
					Result.PathData[Index].Location,
					Direction.Rotation(),
					false,
					nullptr,
					ETeleportType::TeleportPhysics
				);

				Line->SetActorHiddenInGame(false);
			}


			for (int32 Index = RequiredCount;
				Index < ArrPredictLine.Num();
				++Index)
			{
				if (IsValid(ArrPredictLine[Index]))
				{
					ArrPredictLine[Index]->SetActorHiddenInGame(true);
				}
			}

			if (PredictionPoint)
			{
				AtkVelocity = OutVelocity;
				PredictionPoint->SetActorLocation(HitPos);
				PredictionPoint->SetActorHiddenInGame(false);
			}
		}
	}
}

// 生成済みの予測スプラインと表示用構成要素を削除する。
void ALongDistAtkEnemy::DestroyPredictionSpline()
{
	bIsPredictionOn = false;

	if (SplineComp->GetNumberOfSplinePoints() > 0)
		SplineComp->ClearSplinePoints();

	DestroyPredictionSplineMeshes();

	if (PredictionPoint)
		PredictionPoint->Destroy();
}

// 予測線に使用したスプラインメッシュ構成要素をすべて削除する。
void ALongDistAtkEnemy::DestroyPredictionSplineMeshes()
{
	for (int i = 0; i < ArrPredictLine.Num(); i++)
	{
		ArrPredictLine[i]->Destroy();
	}

	ArrPredictLine.Empty();
}

// 線を取得し、呼び出し元へ返す。
ALongdistAtkPredictLine* ALongDistAtkEnemy::GetOrCreatePredictLine(int32 Index)
{
	if (!PredictLineClass)
	{
		return nullptr;
	}

	if (ArrPredictLine.IsValidIndex(Index) &&
		IsValid(ArrPredictLine[Index]))
	{
		return ArrPredictLine[Index];
	}

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ALongdistAtkPredictLine* NewLine =
		GetWorld()->SpawnActor<ALongdistAtkPredictLine>(
			PredictLineClass,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			Params
		);

	if (NewLine)
	{
		NewLine->SetActorHiddenInGame(true);
		ArrPredictLine.Add(NewLine);
	}

	return NewLine;
}

// 衝突後の速度を保存し、跳ね返り状態へ切り替える。
void ALongDistAtkEnemy::KeepSettingVelocity_Implementation(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	UE_LOG(LogTemp, Log, TEXT("AZakoGranade::KeepSettingVelocity"));

	bIsBounced = true;
	AtkVelocity = ImpactVelocity;
}

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void ALongDistAtkEnemy::OnOverlapsBegin_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

// 重なりを検知した相手の種類を確認し、該当するダメージまたは取得効果を適用する。
void ALongDistAtkEnemy::OnExCollOverlapsBegin_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AttributeSet->SetHp(0.0f);
	Death();
}
