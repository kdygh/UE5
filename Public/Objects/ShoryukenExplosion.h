// ゲーム内オブジェクトの動作を処理する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Objects/PooledActor.h"
#include "ShoryukenExplosion.generated.h"

class UNiagaraComponent;
class UCapsuleComponent;
class USceneComponent;

// ゲーム内オブジェクトの動作を処理する役割を持つ。
UCLASS()
class PJ26_API AShoryukenExplosion : public APooledActor
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	AShoryukenExplosion();

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;

	// プールから取得したアクターの位置と状態を再設定し、再び有効にする。
	virtual void ActivateFromPool(const FTransform& SpawnTransform) override;

public:

	// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
	virtual void Tick(float DeltaTime) override;

	// 爆発を生成し、位置・所有者・初期状態を設定する。
	void GenerateExplosion();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;

	// 現在は使用していない処理。
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion", meta = (AllowPrivateAccess = "true"))
	//UNiagaraComponent* NiagaraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion", meta = (AllowPrivateAccess = "true"))
	float Radius = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	float ExplosionDamage = 10.0f;

	FTimerHandle ExplosionTimerHandle;
};
