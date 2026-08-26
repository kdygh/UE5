// 敵固有の行動と戦闘を処理する。

#pragma once

#include "CoreMinimal.h"
#include "../EnemyOrigin.h"
#include "GameFramework/Character.h"
#include "LongDistAtkEnemy.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class ALongdistAtkPredictLine;
class USplineComponent;
class ALongDistAtkPredictPoint;
class UMyAnimInstancePj26;
class AEnemyExplosionActor;

// 敵固有の行動と戦闘を処理する役割を持つ。
UCLASS()
class PJ26_API ALongDistAtkEnemy : public AEnemyOrigin
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	ALongDistAtkEnemy();

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;
	// 構成要素の生成完了後に、相互参照とイベントを接続する。
	virtual void PostInitializeComponents() override;
	// 画面表示を現在とは別の状態または候補へ切り替える。
	virtual void ToggleUI() override;
	// 生存状態を解除し、移動・衝突・戦闘を停止した後に死亡演出を開始する。
	virtual void Death() override;
	// 衝突相手とダメージ量を確認し、プレイヤーへ通常の衝突ダメージを適用する。
	virtual void CalCollisionDamage(AActor* OtherActor) override;
	// 体力を現在の実行状態に合わせて再計算し、関連要素へ反映する。
	virtual void UpdateHealth(float changedHP) override;
	// 体力割合を取得し、呼び出し元へ返す。
	virtual float GetHpPercent() const override;
	// 体力を指定された値へ更新する。
	virtual void SetHP(float NewHP) override;

public:

	// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
	virtual void Tick(float DeltaTime) override;


	// 移動・視点・戦闘に使用する入力と処理関数を関連付ける。
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 現在の戦闘状態と入力条件を確認し、対応する攻撃処理を実行する。
	virtual void MeleeAttack() override;

public:
	// 衝突時の速度を受け取り、跳ね返り後も移動方向を維持する。
	UFUNCTION(BlueprintNativeEvent, Category = "Hit")
	void KeepSettingVelocity(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	// 重なりを検知した相手の種類を確認し、該当するダメージまたは取得効果を適用する。
	UFUNCTION(BlueprintNativeEvent, Category = "Hit")
	void OnOverlapsBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 重なりを検知した相手の種類を確認し、該当するダメージまたは取得効果を適用する。
	UFUNCTION(BlueprintNativeEvent, Category = "Hit")
	void OnExCollOverlapsBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 予測軌道の計算に使用するスプラインと表示用構成要素を生成する。
	UFUNCTION(BlueprintCallable, Category = "PredictionSpline")
	void CreatePredictionSpline();

	// 計算した投射経路に沿ってスプラインメッシュを配置し、予測線を表示する。
	UFUNCTION(BlueprintCallable, Category = "PredictionSpline")
	void DrawPredictionSpline();

	// 生成済みの予測スプラインと表示用構成要素を削除する。
	UFUNCTION(BlueprintCallable, Category = "PredictionSpline")
	void DestroyPredictionSpline();

	// 予測線に使用したスプラインメッシュ構成要素をすべて削除する。
	UFUNCTION(BlueprintCallable, Category = "PredictionSplineMeshes")
	void DestroyPredictionSplineMeshes();

	// 線を取得し、呼び出し元へ返す。
	ALongdistAtkPredictLine* GetOrCreatePredictLine(int32 Index);

	// プレイヤーの現在位置と移動方向を使用し、攻撃目標地点を計算する。
	void SearchPlayerPos();

	// 計算した速度を適用し、敵自身を目標地点の方向へ発射する。
	void ThrowYourself(const FTransform& _transform, const FVector& _pos, const FVector& _velocity);

private:
	UPROPERTY(VisibleAnywhere)
	UMyAnimInstancePj26* AnimInst;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	USphereComponent* ExplosionCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PredictionSpline", meta = (AllowPrivateAccess = "true"))
	USplineComponent* SplineComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PredictLine", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ALongdistAtkPredictLine> PredictLineClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PredictLine", meta = (AllowPrivateAccess = "true"))
	TArray<ALongdistAtkPredictLine*> ArrPredictLine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PredictionPoint", meta = (AllowPrivateAccess = "true"))
	ALongDistAtkPredictPoint* PredictionPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AEnemyExplosionActor> ExplosionActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CameraShake", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraShakeBase> ExplosionCameraShakeClass;

private:
	FVector AtkVelocity;

	bool bIsPredictionOn;

	bool bIsBounced;
};
