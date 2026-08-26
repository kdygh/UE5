// 敵固有の行動と戦闘を処理する。

#include "Enemy/EnemyLaser.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInstance.h"
#include "Materials/Material.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "../../MyPlayer.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
AEnemyLaser::AEnemyLaser()
{

	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("root"));

	SetRootComponent(Root);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AimLine"));

	auto mesh = ConstructorHelpers::FObjectFinder<UStaticMesh>
		(TEXT("StaticMesh'/Game/characters/enemy/enemylaser/Cylinder001.Cylinder001'"));

	if (mesh.Object != nullptr)
	{
		MeshComp->SetStaticMesh(mesh.Object);
		MeshComp->AddRelativeRotation(FRotator(-90.f, 0.f, 0.f));
		MeshComp->SetWorldScale3D(FVector(5.f, 5.f, 5.f));
		MeshComp->SetCollisionProfileName("NoCollision");
		MeshComp->SetupAttachment(Root);
	}

	AimPointComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AimPoint"));

	auto mesh2 = ConstructorHelpers::FObjectFinder<UStaticMesh>
		(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));

	if (mesh2.Object != nullptr)
	{
		AimPointComp->SetStaticMesh(mesh2.Object);
		AimPointComp->SetCollisionProfileName("NoCollision");
		AimPointComp->SetVisibility(false);
	}

	auto mtrl = ConstructorHelpers::FObjectFinder<UMaterial>
		(TEXT("Material'/Game/characters/enemy/enemylaser/LaserMtrl.LaserMtrl'"));

	if (mtrl.Succeeded())
	{
		AimPointComp->SetMaterial(0, mtrl.Object);
		MeshComp->SetMaterial(0, mtrl.Object);
	}


	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(50.f);
	Collision->SetupAttachment(AimPointComp);
	Collision->SetCollisionProfileName("EnemyWeapon");
}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void AEnemyLaser::BeginPlay()
{
	Super::BeginPlay();

}

// 構成要素の生成完了後に、相互参照とイベントを接続する。
void AEnemyLaser::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Collision)
	{
		Collision->OnComponentBeginOverlap.__Internal_AddDynamic(this, &AEnemyLaser::OnOverlapsBegin, FName("OnOverlapsBegin"));
	}
}


// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void AEnemyLaser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// レーザー方向へ線形衝突判定を行い、命中対象へダメージを適用する。
void AEnemyLaser::ShootLaserTrace(const FVector& pos, const FRotator& rot)
{
	SetActorLocation(pos);
	SetActorRotation(rot);

	FVector startloc = GetActorLocation();
	FVector endloc = startloc + (GetActorForwardVector() * 5000.f);
	FHitResult hitResult;

	FCollisionQueryParams objectParams;
	objectParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(hitResult, startloc, endloc, ECollisionChannel::ECC_WorldDynamic);

	if (hitResult.GetActor())
	{
		float hitDist = hitResult.Distance;

		FVector length = FVector(5.f, 5.f, hitDist / 50.f);
		MeshComp->SetWorldScale3D(length);

		AimPointComp->SetVisibility(true);
		AimPointComp->SetWorldLocation(hitResult.ImpactPoint);

		if(AMyPlayer* Player = Cast<AMyPlayer>(hitResult.GetActor()))
		{
			Player->CalCollisionDamage(0.3f, this);
		}
	}
	else
	{
		MeshComp->SetWorldScale3D(FVector(5.f, 5.f, 100.f));
		AimPointComp->SetVisibility(false);
	}
}

// ゲーム全体で使用する参照と初期状態を準備する。
void AEnemyLaser::Init(FVector pos)
{
	SetActorLocation(pos);
}

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void AEnemyLaser::OnOverlapsBegin_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}
