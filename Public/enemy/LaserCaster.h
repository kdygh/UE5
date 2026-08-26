// 敵固有の行動と戦闘を処理する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Objects/PooledActor.h"
#include "LaserCaster.generated.h"

class ALaserRotation;
class AEnemyLaser;

// 敵固有の行動と戦闘を処理する役割を持つ。
UCLASS()
class PJ26_API ALaserCaster : public APooledActor
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	ALaserCaster();

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;

	// 対象に関するイベントを受け取り、関連状態を更新する。
	virtual void OnConstruction(const FTransform& Transform) override;

	// プールから取得したアクターの位置と状態を再設定し、再び有効にする。
	virtual void ActivateFromPool(const FTransform& SpawnTransform) override;

public:

	// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
	virtual void Tick(float DeltaTime) override;


	// 設定された数と角度に合わせて、レーザー回転構成要素を再生成する。
	void RebuildLaserRotations();

	// 動的に生成したレーザー回転構成要素を削除し、一覧を空にする。
	void ClearLaserRotations();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root", meta = (MakeEditWidget = "true", AllowPrivateAccess = "true"))
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (MakeEditWidget = "true", AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (MakeEditWidget = "true", AllowPrivateAccess = "true"))
	int32 LaserCount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (MakeEditWidget = "true", AllowPrivateAccess = "true"))
	float AngleStep = 72.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser", meta = (MakeEditWidget = "true", AllowPrivateAccess = "true"))
	float rotateSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Laser", meta = (MakeEditWidget = "true", AllowPrivateAccess = "true"))
	TArray<TObjectPtr<AEnemyLaser>> Lasers;

	UPROPERTY(Transient)
	TArray<TObjectPtr<USceneComponent>> LaserRotations;

	UPROPERTY()
	TArray<FRotator> BaseLaserRotations;

private:
	float angle;
};
