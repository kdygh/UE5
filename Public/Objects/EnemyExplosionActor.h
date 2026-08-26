// ゲーム内オブジェクトの動作を処理する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Objects/PooledActor.h"
#include "EnemyExplosionActor.generated.h"

class UNiagaraComponent;
class UCapsuleComponent;
class USceneComponent;
class AEnemyOrigin;

// ゲーム内オブジェクトの動作を処理する役割を持つ。
UCLASS()
class PJ26_API AEnemyExplosionActor : public APooledActor
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	AEnemyExplosionActor();

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;

public:

	// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
	virtual void Tick(float DeltaTime) override;
	// 範囲内の対象を取得し、重複を避けながらダメージまたは効果を適用する。
	void PerformOverlapAction();

private:
	float currentTime = 0.f;
	bool bIsActive = false;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	float Radius = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	float ExplosionDamage = 30.0f;

private:
	// 現在は使用していない処理。
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shockwave", meta = (AllowPrivateAccess = "true"))
	//UNiagaraComponent* NiagaraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;

	AEnemyOrigin* Owner;
};
