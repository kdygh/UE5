// アニメーションとゲーム処理を連携する。

#include "MyAnimNotify.h"
#include "MyPlayer.h"
#include "enemy/NormalEnemy.h"
#include "enemy/WeakEnemy.h"
#include "EnemyOrigin.h"

// アニメーション通知を受け取り、所有キャラクターへ対応する動作を伝える。
void UMyAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp) return;

    if (AMyPlayer* Character = Cast<AMyPlayer>(MeshComp->GetOwner()))
    {
        Character->OnAnimAction(ActionName);
    }

    if(AEnemyOrigin* Enemy = Cast<AEnemyOrigin>(MeshComp->GetOwner()))
    {
        Enemy->OnAnimAction(ActionName);
	}

 // 現在は使用していない処理。
 //   if(AWeakEnemy* Enemy = Cast<AWeakEnemy>(MeshComp->GetOwner()))
 //   {
 //       Enemy->OnAnimAction(ActionName);
	//}
}
