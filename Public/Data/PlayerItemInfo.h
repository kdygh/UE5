// アイテムの取得効果を処理する。

#pragma once

#include "CoreMinimal.h"
#include "PlayerItemInfo.generated.h"

class AItem;


// アイテムの取得効果を処理する役割を持つ。
USTRUCT(BlueprintType)
struct FPlayerItemInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString ItemDesc;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Amount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AItem> ItemClass;
};
