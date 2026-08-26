// 敵固有の行動と戦闘を処理する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaserRotation.generated.h"

class AEnemyLaser;
class UStaticMeshComponent;
class USceneComponent;

// 敵固有の行動と戦闘を処理する役割を持つ。
UCLASS()
class PJ26_API ALaserRotation : public AActor
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	ALaserRotation();

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;

public:

	// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root", meta = (MakeEditWidget = "true", AllowPrivateAccess = "true"))
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root", meta = (MakeEditWidget = "true", AllowPrivateAccess = "true"))
	USceneComponent* LaserSceneComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (MakeEditWidget = "true", AllowPrivateAccess = "true"))
	UMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser", meta = (MakeEditWidget = "true", AllowPrivateAccess = "true"))
	AEnemyLaser* Laser;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser", meta = (MakeEditWidget = "true", AllowPrivateAccess = "true"))
	float rotateSpeed;

	float angle;

};
