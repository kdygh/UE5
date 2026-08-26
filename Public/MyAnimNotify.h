// アニメーションとゲーム処理を連携する。

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "MyAnimNotify.generated.h"


// アニメーションとゲーム処理を連携する役割を持つ。
UCLASS()
class PJ26_API UMyAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action")
    FName ActionName;

    virtual void Notify(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation
    ) override;
};
