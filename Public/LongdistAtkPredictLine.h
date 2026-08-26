// ゲーム機能の状態と処理を定義する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LongdistAtkPredictLine.generated.h"

class USplineMeshComponent;
class UMaterial;

// ゲーム機能の状態と処理を定義する役割を持つ。
UCLASS()
class PJ26_API ALongdistAtkPredictLine : public AActor
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	ALongdistAtkPredictLine();

	// ゲーム全体で使用する参照と初期状態を準備する。
	void Init(FVector pos, FRotator rot);

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;

public:

	// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ThrowSpeed", meta = (AllowPrivateAccess = "true"))
	USplineMeshComponent* MeshComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ThrowSpeed", meta = (AllowPrivateAccess = "true"))
	UMaterial* Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;

};
