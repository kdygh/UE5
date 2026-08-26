// ゲーム内オブジェクトの動作を処理する。

#include "Objects/Wall.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
AWall::AWall()
{

	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallMesh"));
	WallMesh->SetupAttachment(Root);

	WallMaterial = CreateDefaultSubobject<UMaterialInterface>(TEXT("WallMaterial"));
}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void AWall::BeginPlay()
{
	Super::BeginPlay();

}


// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void AWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
