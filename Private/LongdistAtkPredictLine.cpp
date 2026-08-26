// ゲーム機能の状態と処理を定義する。

#include "LongdistAtkPredictLine.h"
#include "Components/SplineMeshComponent.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
ALongdistAtkPredictLine::ALongdistAtkPredictLine()
{

	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("root"));
	SetRootComponent(Root);

	MeshComp = CreateDefaultSubobject<USplineMeshComponent>(TEXT("PredictLine"));

	if(MeshComp)
	{
		MeshComp->SetStaticMesh(MeshComp->GetStaticMesh());
		MeshComp->SetWorldScale3D(FVector(0.3f, 0.3f, 0.3f));
		MeshComp->SetCollisionProfileName("NoCollision");
		MeshComp->SetForwardAxis(ESplineMeshAxis::Z);
		// 現在は使用していない処理。
		//MeshComp->SetupAttachment(RootComponent);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeshComp->SetupAttachment(Root);
	}
}

// ゲーム全体で使用する参照と初期状態を準備する。
void ALongdistAtkPredictLine::Init(FVector pos, FRotator rot)
{
	MeshComp->SetWorldLocation(pos);
	MeshComp->SetWorldRotation(rot.Quaternion());
}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void ALongdistAtkPredictLine::BeginPlay()
{
	Super::BeginPlay();

}


// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void ALongdistAtkPredictLine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
