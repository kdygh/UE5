// ゲームで使用する設定データを定義する。

#pragma once

#include "CoreMinimal.h"
#include "PlayerInfo.generated.h"


// ゲームで使用する設定データを定義する役割を持つ。
USTRUCT(BlueprintType)
struct FPlayerInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName PlayerName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AtkPower;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SPAtkPower;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Def;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HP;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SP;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ThunderDmg;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float WindDmg;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FireDmg;

};
