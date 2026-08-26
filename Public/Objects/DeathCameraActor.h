// ゲーム内オブジェクトの動作を処理する。

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "DeathCameraActor.generated.h"


class USkeletalMeshComponent;

// ゲーム内オブジェクトの動作を処理する役割を持つ。
UCLASS()
class PJ26_API ADeathCameraActor : public ACameraActor
{
	GENERATED_BODY()

public:
    // 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
    ADeathCameraActor();

    // 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
    virtual void Tick(float DeltaTime) override;

    // 対象を指定された値へ更新する。
    void SetTarget(USkeletalMeshComponent* InTarget);

private:
    TWeakObjectPtr<USkeletalMeshComponent> TargetMesh;

    FVector CameraOffset =
        FVector(-350.f, 150.f, 150.f);
};
