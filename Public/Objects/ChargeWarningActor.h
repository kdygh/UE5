// ゲーム内オブジェクトの動作を処理する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChargeWarningActor.generated.h"

// ゲーム内オブジェクトの動作を処理する役割を持つ。
UCLASS()
class PJ26_API AChargeWarningActor : public AActor
{
	GENERATED_BODY()

public:
    // 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
    AChargeWarningActor();

    void InitializeWarning(
        AActor* InSource,
        AActor* InTarget,
        float InDuration
    );

    // 対象に関するイベントを受け取り、関連状態を更新する。
    virtual void OnConstruction(const FTransform& Transform) override;

protected:
    // 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UDecalComponent> DecalComp;

    UPROPERTY(EditAnywhere, Category = "Arrow Decal")
    TObjectPtr<UMaterialInterface> BaseMaterial;

    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> MID;

    UPROPERTY(EditAnywhere, Category = "Arrow Decal")
    float DecalLength = 800.f;

    UPROPERTY(EditAnywhere, Category = "Arrow Decal")
    float DecalWidth = 200.f;

    UPROPERTY(EditAnywhere, Category = "Arrow Decal")
    float ArrowTileLength = 100.f;

    UPROPERTY(EditAnywhere, Category = "Arrow Decal")
    float ArrowTileWidth = 100.f;

    UPROPERTY(EditAnywhere, Category = "Arrow Decal")
    float ScrollSpeed = 1.0f;

    UPROPERTY()
    TObjectPtr<AActor> SourceActor;

    UPROPERTY()
    TObjectPtr<AActor> TargetActor;

    UPROPERTY(EditAnywhere, Category = "Warning")
    float WarningWidth = 120.f;

    float RemainingTime = 0.f;

};
