// ゲーム内オブジェクトの動作を処理する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Wall.generated.h"

// ゲーム内オブジェクトの動作を処理する役割を持つ。
UCLASS()
class PJ26_API AWall : public AActor
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	AWall();

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;

public:

	// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WallMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Material", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* WallMaterial;
};
