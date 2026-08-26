// ゲームで使用する設定データを定義する。

#pragma once

#include "CoreMinimal.h"
#include "../EnemyOrigin.h"
#include "EnemyInfo.generated.h"


// ゲームで使用する設定データを定義する役割を持つ。
USTRUCT(BlueprintType)
struct FEnemyInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName EnemyName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AtkPower;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Def;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HP;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SP;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName DropItem1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName DropItem2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName DropItem3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ItemDropPercentage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AEnemyOrigin> EnemyClass;
};
