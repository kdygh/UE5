// アイテムの取得効果を処理する。

#include "PlayerItem/PlayerItem.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
APlayerItem::APlayerItem()
{

	PrimaryActorTick.bCanEverTick = true;

}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void APlayerItem::BeginPlay()
{
	Super::BeginPlay();

}


// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void APlayerItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
