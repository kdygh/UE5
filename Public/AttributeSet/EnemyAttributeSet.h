// 能力値の保持と変更を処理する。

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "EnemyAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


// 能力値の保持と変更を処理する役割を持つ。
UCLASS()
class PJ26_API UEnemyAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	// ゲーム効果適用後の能力値を上限内に補正し、ダメージや死亡状態を反映する。
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Hp;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, Hp);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxHp;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, MaxHp);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Sp;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, Sp);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxSp;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, MaxSp);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData AtkPower;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, AtkPower);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Def;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, Def);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, MoveSpeed);

	UPROPERTY(BlueprintReadOnly, Category = "Meta")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, Damage)
};
