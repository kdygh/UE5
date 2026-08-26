// 敵固有の行動と戦闘を処理する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyLaser.generated.h"

// 敵固有の行動と戦闘を処理する役割を持つ。
UCLASS()
class PJ26_API AEnemyLaser : public AActor
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	AEnemyLaser();

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;
	// 構成要素の生成完了後に、相互参照とイベントを接続する。
	virtual void PostInitializeComponents() override;

public:

	// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
	virtual void Tick(float DeltaTime) override;

	// レーザー方向へ線形衝突判定を行い、命中対象へダメージを適用する。
	void ShootLaserTrace(const FVector& pos, const FRotator& rot);

	// ゲーム全体で使用する参照と初期状態を準備する。
	void Init(FVector pos);

	// 重なりを検知した相手の種類を確認し、該当するダメージまたは取得効果を適用する。
	UFUNCTION(BlueprintNativeEvent, Category = "Hit")
	void OnOverlapsBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AimLaser", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* MeshComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AimPoint", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* AimPointComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* Collision;
};
