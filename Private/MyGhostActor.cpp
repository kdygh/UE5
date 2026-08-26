// ゲーム機能の状態と処理を定義する。

#include "MyGhostActor.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
AMyGhostActor::AMyGhostActor()
{

    PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GhostMesh"));
    RootComponent = Mesh;


    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void AMyGhostActor::BeginPlay()
{
	Super::BeginPlay();
}
