// 敵固有の行動と戦闘を処理する。

#include "enemy/LaserCaster.h"
#include "enemy/LaserRotation.h"
#include "enemy/EnemyLaser.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "../../MyPlayer.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
ALaserCaster::ALaserCaster()
{

	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("root"));

	SetRootComponent(Root);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserCaster"));
	MeshComp->SetupAttachment(Root);
}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void ALaserCaster::BeginPlay()
{
	Super::BeginPlay();
	RebuildLaserRotations();
}

// 対象に関するイベントを受け取り、関連状態を更新する。
void ALaserCaster::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

// プールから取得したアクターの位置と状態を再設定し、再び有効にする。
void ALaserCaster::ActivateFromPool(const FTransform& SpawnTransform)
{
	bInUse = true;

	SetActorTransform(SpawnTransform);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);

	RebuildLaserRotations();
}


// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void ALaserCaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AMyPlayer* Player = Cast<AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		if (Player->GetIsAlive() == false)
		{
			ClearLaserRotations();
			DeactivateToPool();
			return;
		}
	}

	if (currentTime >= Duration)
	{
		currentTime = 0.0f;
		ClearLaserRotations();
		DeactivateToPool();
		return;
	}

	currentTime += DeltaTime;

	// 現在は使用していない処理。
	//angle = DeltaTime * rotateSpeed;

	// 現在は使用していない処理。
	//FRotator deltaRotation = FRotator(0, angle, 0);
	//AddActorWorldRotation(deltaRotation);

	FVector NewLocation = GetActorForwardVector() * MoveSpeed * DeltaTime;
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("ALaserCaster MoveDirection: %s"), *NewLocation.ToString()));
	AddActorWorldOffset(NewLocation, true);


	if (!Lasers.IsEmpty() && !LaserRotations.IsEmpty())
	{
		for (int32 i = 0; i < Lasers.Num(); ++i)
		{
			if (!Lasers[i] || !LaserRotations.IsValidIndex(i) || !LaserRotations[i] || !BaseLaserRotations.IsValidIndex(i))
			{
				continue;
			}

			const float Frequency = 2.0f;
			const float Amplitude = 60.0f;

			const float Phase = i * 0.7f;
			const float SineOffset = FMath::Sin(currentTime * Frequency + Phase) * Amplitude;

			FRotator NewRot = BaseLaserRotations[i];
			NewRot.Pitch += SineOffset;

			LaserRotations[i]->SetRelativeRotation(NewRot);

			Lasers[i]->ShootLaserTrace(
				LaserRotations[i]->GetComponentLocation(),
				LaserRotations[i]->GetComponentRotation()
			);
		}
	}
}

// 設定された数と角度に合わせて、レーザー回転構成要素を再生成する。
void ALaserCaster::RebuildLaserRotations()
{
	ClearLaserRotations();

	if (!Root)
	{
		UE_LOG(LogTemp, Warning, TEXT("Root component is null. Cannot rebuild laser rotations."));
		return;
	}

	for (int32 i = 0; i < LaserCount; ++i)
	{
		const FString CompName = FString::Printf(TEXT("LaserRotation_%d"), i);

		USceneComponent* LaserRotationComp = NewObject<USceneComponent>(this, USceneComponent::StaticClass(), *CompName);
		if (!LaserRotationComp)
		{
			continue;
		}

		LaserRotationComp->SetupAttachment(Root);

		AEnemyLaser* Laser = Cast<AEnemyLaser>(
			UGameplayStatics::BeginDeferredActorSpawnFromClass(
				this,
				AEnemyLaser::StaticClass(),
				LaserRotationComp->GetComponentTransform()
			)
		);

		if (Laser)
		{
			UGameplayStatics::FinishSpawningActor(Laser, LaserRotationComp->GetComponentTransform());

			Laser->AttachToComponent(
				LaserRotationComp,
				FAttachmentTransformRules::SnapToTargetNotIncludingScale
			);

			Laser->ShootLaserTrace(
				LaserRotationComp->GetComponentLocation(),
				LaserRotationComp->GetComponentRotation()
			);
		}

		Lasers.Add(Laser);

		LaserRotationComp->RegisterComponent();


		const float Yaw = AngleStep * i;
		LaserRotationComp->SetRelativeRotation(FRotator(0.f, Yaw, 0.f));


		AddInstanceComponent(LaserRotationComp);

		LaserRotations.Add(LaserRotationComp);
		BaseLaserRotations.Add(FRotator(0.f, Yaw, 0.f));

		UE_LOG(LogTemp, Warning, TEXT("Created LaserRotation_%d"), i);
	}
}

// 動的に生成したレーザー回転構成要素を削除し、一覧を空にする。
void ALaserCaster::ClearLaserRotations()
{
	for (USceneComponent* Comp : LaserRotations)
	{
		if (Comp)
		{
			Comp->DestroyComponent();
		}
	}

	for (AEnemyLaser* Laser : Lasers)
	{
		if (Laser)
		{
			Laser->Destroy();
		}
	}

	Lasers.Empty();
	LaserRotations.Empty();
}
