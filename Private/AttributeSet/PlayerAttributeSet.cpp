// 能力値の保持と変更を処理する。

#include "AttributeSet/PlayerAttributeSet.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectExtension.h"

// ゲーム効果適用後の能力値を上限内に補正し、ダメージや死亡状態を反映する。
void UPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float LocalDamage = GetDamage();
		SetDamage(0.f);

		if (LocalDamage > 0.f)
		{
			const float NewHealth = FMath::Clamp(GetHp() - LocalDamage, 0.f, GetMaxHp());
			SetHp(NewHealth);
		}
	}
}
