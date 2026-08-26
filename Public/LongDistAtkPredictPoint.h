// ゲーム機能の状態と処理を定義する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LongDistAtkPredictPoint.generated.h"

class UStaticMeshComponent;
class UMaterial;

// ゲーム機能の状態と処理を定義する役割を持つ。
UCLASS()
class PJ26_API ALongDistAtkPredictPoint : public AActor
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	ALongDistAtkPredictPoint();

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;

public:

	// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
	virtual void Tick(float DeltaTime) override;

	// 対象の参照と初期状態を設定し、使用可能な状態にする。
	void Initialize(const FVector& pos);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	UMaterial* Material;

	FVector VecVelocity;
};
