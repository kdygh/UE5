// ゲーム機能の状態と処理を定義する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyGhostActor.generated.h"

// ゲーム機能の状態と処理を定義する役割を持つ。
UCLASS()
class PJ26_API AMyGhostActor : public AActor
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	AMyGhostActor();

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GhostTrailMaterial")
	UMaterial* GhostTrailMaterial;

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;
};
