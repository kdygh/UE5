// 敵固有の行動と戦闘を処理する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../EnemyOrigin.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "../Objects/ChargeWarningActor.h"
#include "DashEnemy.generated.h"

// 敵固有の行動と戦闘を処理する役割を持つ。
UCLASS()
class PJ26_API ADashEnemy : public AEnemyOrigin
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	ADashEnemy();

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;
	// 画面表示を現在とは別の状態または候補へ切り替える。
	virtual void ToggleUI() override;
	// 生存状態を解除し、移動・衝突・戦闘を停止した後に死亡演出を開始する。
	virtual void Death() override;
	// 衝突相手とダメージ量を確認し、プレイヤーへ通常の衝突ダメージを適用する。
	virtual void CalCollisionDamage(AActor* OtherActor) override;
	// 衝突相手とダメージ量を確認し、プレイヤーへ通常の衝突ダメージを適用する。
	virtual void CalCollisionDamage(float DamageAmount) override;
	// 衝突相手を確認し、プレイヤーへ属性またはスタミナに関するダメージを適用する。
	virtual void CalCollisionDamageSP(float DamageAmount) override;
	// 被弾状態へ切り替え、攻撃中断・アニメーション・関連演出を処理する。
	virtual void Damaged() override;
	// 気絶状態へ切り替え、現在の行動を停止して気絶演出を開始する。
	virtual void Stun() override;
	// 体力を現在の実行状態に合わせて再計算し、関連要素へ反映する。
	virtual void UpdateHealth(float changedHP) override;
	// 構成要素の生成完了後に、相互参照とイベントを接続する。
	virtual void PostInitializeComponents() override;
	// 対象とプレイヤーの時間倍率を下げ、タイマーで通常速度への復帰を予約する。
	virtual void DoHitStop(float Duration) override;
	// 一時的に変更した時間倍率を通常の値へ戻す。
	virtual void ResetTimeDilation(float Duration) override;
	// 命中を指定された値へ更新する。
	virtual void SetIsHitByObject(bool _isHit) override;

	// 体力割合を取得し、呼び出し元へ返す。
	virtual float GetHpPercent() const override;
	// スタミナ割合を取得し、呼び出し元へ返す。
	virtual float GetSpPercent() const override;
	// 体力を指定された値へ更新する。
	virtual void SetHP(float NewHP) override;
	// スタミナを指定された値へ更新する。
	virtual void SetSP(float NewSP) override;

public:

	// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
	virtual void Tick(float DeltaTime) override;


	// 移動・視点・戦闘に使用する入力と処理関数を関連付ける。
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// プレイヤーに関するイベントを受け取り、関連状態を更新する。
	UFUNCTION(BlueprintCallable, Category = "PawnSensing")
	virtual void OnFollowPlayer(bool _isPlayerInSight) override;

	// 現在の戦闘状態と入力条件を確認し、対応する攻撃処理を実行する。
	virtual void MeleeAttack() override;

	// アニメーションモンタージュを取得し、呼び出し元へ返す。
	virtual UAnimMontage* GetAnimMontage() { return AtkMontage; }

	// ラグドール移行後にアニメーション更新を停止し、物理姿勢だけを維持する。
	virtual void DisableAnimationAfterRagdoll() override;

	// アニメーション通知名に応じて、攻撃判定・回避・パリィ・死亡状態を切り替える。
	virtual void OnAnimAction(FName ActionName) override;

	// プレイヤーの現在位置と移動方向を使用し、攻撃目標地点を計算する。
	void SearchPlayerPos();

	// 目標位置の検索を終了し、関連するタイマーと状態を初期化する。
	void StopSearching();


	// 予測軌道の計算に使用するスプラインと表示用構成要素を生成する。
	UFUNCTION(BlueprintCallable, Category = "PredictionSpline")
	void CreatePredictionSpline();

	// 計算した投射経路に沿ってスプラインメッシュを配置し、予測線を表示する。
	UFUNCTION(BlueprintCallable, Category = "PredictionSpline")
	void DrawPredictionSpline();

public:
	// 攻撃開始時に武器の衝突判定を有効にし、直前のソケット位置を記録する。
	void StartTraceAttack();
	// 攻撃終了時に武器の衝突判定を停止し、命中対象の記録を初期化する。
	void EndTraceAttack();

	// 重なりを検知した相手の種類を確認し、該当するダメージまたは取得効果を適用する。
	UFUNCTION(BlueprintNativeEvent, Category = "Hit")
	void OnOverlapsBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// 前回と現在の武器位置の間をスイープし、高速な攻撃の衝突を検出する。
	void PerformWeaponTrace();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AtkMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DmgAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathAnimMontage;

	UPROPERTY(VisibleAnywhere)
	class UMyAnimInstancePj26* AnimInst;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* DashCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PredictionSpline", meta = (AllowPrivateAccess = "true"))
	USplineComponent* SplineComp;

	// 現在は使用していない処理。
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PredictionSplineMeshes", meta = (AllowPrivateAccess = "true"))
	//TArray<Achargedirection*> ArrSplineMeshesComp;

private:
	bool IsPredictionOn = false;
	bool bIsHitPlayer = false;
	float DashTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	FName TraceStartSocket = TEXT("pelvis");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	float TraceRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_GameTraceChannel7;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	bool bDrawDebugTrace = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge", meta = (AllowPrivateAccess = "true"))
	AChargeWarningActor* ChargeWarningActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Charge", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AChargeWarningActor> ChargeWarningClass;

	UPROPERTY()
	FVector PrevStart = FVector::ZeroVector;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActors;
};
