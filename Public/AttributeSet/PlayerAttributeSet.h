// 能力値の保持と変更を処理する。

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "PlayerAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


// 能力値の保持と変更を処理する役割を持つ。
UCLASS()
class PJ26_API UPlayerAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	// ゲーム効果適用後の能力値を上限内に補正し、ダメージや死亡状態を反映する。
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Hp;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Hp);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxHp;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxHp);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Sp;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Sp);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxSp;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxSp);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData AtkPower;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, AtkPower);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData SPAtkPower;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, SPAtkPower);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Def;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Def);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MoveSpeed);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData ThunderDmg;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, ThunderDmg);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData WindDmg;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, WindDmg);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData FireDmg;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, FireDmg);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData SpRegenRate;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, SpRegenRate)

	UPROPERTY(BlueprintReadOnly, Category = "Meta")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Damage)
};
