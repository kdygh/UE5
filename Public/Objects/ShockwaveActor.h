// ゲーム内オブジェクトの動作を処理する。

#pragma once

#include "CoreMinimal.h"
#include "Objects/PooledActor.h"
#include "ShockwaveActor.generated.h"

class UNiagaraComponent;
class UCapsuleComponent;
class USceneComponent;

// ゲーム内オブジェクトの動作を処理する役割を持つ。
UCLASS()
class PJ26_API AShockwaveActor : public APooledActor
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	AShockwaveActor();

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;

public:

	// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
	virtual void Tick(float DeltaTime) override;
	// 範囲内の対象を取得し、重複を避けながら衝撃波の効果を適用する。
	void PerformOverlapActtion();

private:
	float currentTime = 0.f;
	bool bIsActive = false;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	float Radius = 300.0f;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shockwave", meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* NiagaraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;
};
