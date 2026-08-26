// ゲーム内オブジェクトの動作を処理する。

#include "Objects/UEnemyAttackQueueComponent.h"


// 攻撃の利用を要求し、取得可能か判定する。
bool UUEnemyAttackQueueComponent::RequestAttack(AEnemyOrigin* Enemy)
{
	return false;
}

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void UUEnemyAttackQueueComponent::ReleaseAttack(AEnemyOrigin* Enemy)
{
}

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void UUEnemyAttackQueueComponent::RemoveEnemy(AEnemyOrigin* Enemy)
{
}

// 攻撃の条件を満たしているか判定する。
bool UUEnemyAttackQueueComponent::HasAttackToken(const AEnemyOrigin* Enemy) const
{
	return false;
}

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void UUEnemyAttackQueueComponent::TryGrantNext()
{
}
