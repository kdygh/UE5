// ゲーム内オブジェクトの動作を処理する。

#include "Objects/PooledActor.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
APooledActor::APooledActor()
{

	PrimaryActorTick.bCanEverTick = false;

}

// プールから取得したアクターの位置と状態を再設定し、再び有効にする。
void APooledActor::ActivateFromPool(const FTransform& SpawnTransform)
{
	bInUse = true;

	SetActorTransform(SpawnTransform);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
}

// 表示・衝突・更新を無効にし、アクターをプールで待機する状態へ戻す。
void APooledActor::DeactivateToPool()
{
	bInUse = false;

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
}
