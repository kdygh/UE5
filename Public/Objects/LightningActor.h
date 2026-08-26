// ゲーム内オブジェクトの動作を処理する。

#pragma once

#include "CoreMinimal.h"
#include "Objects/PooledActor.h"
#include "LightningActor.generated.h"


class UNiagaraComponent;
class UCapsuleComponent;
class USceneComponent;

// ゲーム内オブジェクトの動作を処理する役割を持つ。
UCLASS()
class PJ26_API ALightningActor : public APooledActor
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	ALightningActor();

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;
	// 構成要素の生成完了後に、相互参照とイベントを接続する。
	virtual void PostInitializeComponents() override;

public:

	// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
	virtual void Tick(float DeltaTime) override;

	// 方向を指定された値へ更新する。
	void SetMoveDirection(const FVector& NewDirection) { MoveDirection = NewDirection; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightning", meta = (AllowPrivateAccess = "true"))
	FName CollisionProfileName;

	// 現在は使用していない処理。
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightning", meta = (AllowPrivateAccess = "true"))
	//UNiagaraComponent* NiagaraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightning", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* CapsuleLightning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;
};
