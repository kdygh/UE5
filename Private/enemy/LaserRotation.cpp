// 敵固有の行動と戦闘を処理する。

#include "Enemy/LaserRotation.h"
#include "Enemy/EnemyLaser.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
ALaserRotation::ALaserRotation()
{

	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("root"));

	SetRootComponent(Root);

	LaserSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("laserSceneComp"));
	LaserSceneComp->SetupAttachment(RootComponent);

	// 現在は使用していない処理。
	//MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AimLine"));

	// 現在は使用していない処理。
	//auto mesh = ConstructorHelpers::FObjectFinder<UStaticMesh>
	//	(TEXT("StaticMesh'/Game/MS_ConstructMiscV1/3D_Asset/01_Painted_Pipe_Base_tiwicjehx/tiwicjehx_LOD0.tiwicjehx_LOD0'"));

	// 現在は使用していない処理。
	//if (mesh.Object != nullptr)
	//{
	//	MeshComp->SetStaticMesh(mesh.Object);
		//MeshComp->SetWorldScale3D(FVector(5.f, 5.f, 5.f));
		//MeshComp->SetCollisionProfileName("NoCollision");
		//MeshComp->SetupAttachment(RootComponent);
	//}

	angle = 0.f;
	rotateSpeed = 40.f;
}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void ALaserRotation::BeginPlay()
{
	Super::BeginPlay();

	if (Laser == nullptr)
	{
		Laser = Cast<AEnemyLaser>(
			UGameplayStatics::BeginDeferredActorSpawnFromClass(
				this,
				AEnemyLaser::StaticClass(),
				LaserSceneComp->GetComponentTransform()
			)
		);

		if (Laser)
		{
			UGameplayStatics::FinishSpawningActor(Laser, LaserSceneComp->GetComponentTransform());

			Laser->AttachToComponent(
				LaserSceneComp,
				FAttachmentTransformRules::SnapToTargetNotIncludingScale
			);

			Laser->ShootLaserTrace(
				LaserSceneComp->GetComponentLocation(),
				LaserSceneComp->GetComponentRotation()
			);
		}
	}
}


// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void ALaserRotation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	angle = DeltaTime * rotateSpeed;

	FRotator deltaRotation = FRotator(0, angle, 0);
	AddActorWorldRotation(deltaRotation);

	if (Laser)
	{
		Laser->ShootLaserTrace(LaserSceneComp->GetComponentLocation(), LaserSceneComp->GetComponentRotation());
	}
}
