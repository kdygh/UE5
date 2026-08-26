// ゲーム機能の状態と処理を定義する。

#include "LongDistAtkPredictPoint.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/Material.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
ALongDistAtkPredictPoint::ALongDistAtkPredictPoint()
{

	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Explosive Weapon"));

	Material = CreateDefaultSubobject<UMaterial>(TEXT("Material"));

	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetMaterial(0, Material);

	VecVelocity = FVector::ZeroVector;
}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void ALongDistAtkPredictPoint::BeginPlay()
{
	Super::BeginPlay();

}


// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void ALongDistAtkPredictPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// 対象の参照と初期状態を設定し、使用可能な状態にする。
void ALongDistAtkPredictPoint::Initialize(const FVector& pos)
{
	SetActorLocation(pos);
}
