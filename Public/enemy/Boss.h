// 敵固有の行動と戦闘を処理する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIController.h"
#include "../EnemyOrigin.h"
#include "Boss.generated.h"

class ALaserCaster;
class AShoryukenExplosion;
class UBossStatusWidget;

// 敵固有の行動と戦闘を処理する役割を持つ。
UCLASS()
class PJ26_API ABoss : public AEnemyOrigin
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	ABoss();

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;

	// 生存状態を解除し、移動・衝突・戦闘を停止した後に死亡演出を開始する。
	virtual void Death() override;
	// 衝突相手とダメージ量を確認し、プレイヤーへ通常の衝突ダメージを適用する。
	virtual void CalCollisionDamage(float DamageAmount) override;
	// 衝突相手を確認し、プレイヤーへ属性またはスタミナに関するダメージを適用する。
	virtual void CalCollisionDamageSP(float DamageAmount) override;
	// 気絶状態へ切り替え、現在の行動を停止して気絶演出を開始する。
	virtual void Stun() override;

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

	// ラグドール移行後にアニメーション更新を停止し、物理姿勢だけを維持する。
	virtual void DisableAnimationAfterRagdoll() override;

	// アニメーション通知名に応じて、攻撃判定・回避・パリィ・死亡状態を切り替える。
	void OnAnimAction(FName ActionName);
	// 現在の戦闘状態と入力条件を確認し、対応する攻撃処理を実行する。
	void DashAttack();

public:
	// 左武器の衝突判定を有効にし、直前のソケット位置を記録する。
	void StartTraceAttackL();
	// 左武器の衝突判定を終了し、命中対象の記録を整理する。
	void EndTraceAttackL();
	// 右武器の衝突判定を有効にし、直前のソケット位置を記録する。
	void StartTraceAttackR();
	// 右武器の衝突判定を終了し、命中対象の記録を整理する。
	void EndTraceAttackR();

protected:
	// 左武器の前回・現在位置をスイープし、攻撃の衝突を検出する。
	void PerformWeaponTraceL();
	// 右武器の前回・現在位置をスイープし、攻撃の衝突を検出する。
	void PerformWeaponTraceR();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Laser", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ALaserCaster> LaserCasterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Laser", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AShoryukenExplosion> ShoryukenExplosionClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	FName TraceStartSocketL = TEXT("hand_slide_l");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	FName TraceStartSocketR = TEXT("hand_slide_r");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	float TraceRadius = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_GameTraceChannel3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	bool bDrawDebugTrace = true;

	UPROPERTY()
	FVector PrevStart = FVector::ZeroVector;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActors;

private:
	UPROPERTY(VisibleAnywhere)
	class UMyAnimInstancePj26* AnimInst;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DashMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathAnimMontage;

	bool bIsDashing = false;
	bool bIsHitPlayer = false;
};
