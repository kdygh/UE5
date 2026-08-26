// 敵固有の行動と戦闘を処理する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIController.h"
#include "../EnemyOrigin.h"
#include "NormalEnemy.generated.h"

// 敵固有の行動と戦闘を処理する役割を持つ。
UCLASS()
class PJ26_API ANormalEnemy : public AEnemyOrigin
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	ANormalEnemy();

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;
	// 生存状態を解除し、移動・衝突・戦闘を停止した後に死亡演出を開始する。
	virtual void Death() override;
	// 衝突相手とダメージ量を確認し、プレイヤーへ通常の衝突ダメージを適用する。
	virtual void CalCollisionDamage(float DamageAmount) override;
	// 衝突相手を確認し、プレイヤーへ属性またはスタミナに関するダメージを適用する。
	virtual void CalCollisionDamageSP(float DamageAmount) override;
	// 被弾状態へ切り替え、攻撃中断・アニメーション・関連演出を処理する。
	virtual void Damaged() override;

	// 気絶状態へ切り替え、現在の行動を停止して気絶演出を開始する。
	virtual void Stun() override;
	// 構成要素の生成完了後に、相互参照とイベントを接続する。
	virtual void PostInitializeComponents() override;
	// 対象とプレイヤーの時間倍率を下げ、タイマーで通常速度への復帰を予約する。
	virtual void DoHitStop(float Duration) override;
	// 一時的に変更した時間倍率を通常の値へ戻す。
	virtual void ResetTimeDilation(float Duration) override;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool isMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AtkMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DmgAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathAnimMontage;

	// 重なりを検知した相手の種類を確認し、該当するダメージまたは取得効果を適用する。
	UFUNCTION(BlueprintNativeEvent, Category = "Hit")
	void OnOverlapsBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	// 重なり終了の通知を受け取り、必要な接触状態を解除する。
	UFUNCTION(BlueprintNativeEvent, Category = "Hit")
	void OnOverlapsEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// 重なりを検知した相手の種類を確認し、該当するダメージまたは取得効果を適用する。
	UFUNCTION(BlueprintNativeEvent, Category = "Atk")
	void OnRWeaponOverlapsBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 重なりを検知した相手の種類を確認し、該当するダメージまたは取得効果を適用する。
	UFUNCTION(BlueprintNativeEvent, Category = "Atk")
	void OnLWeaponOverlapsBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

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
	UPROPERTY(VisibleAnywhere)
	class UMyAnimInstancePj26* AnimInst;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* WeaponRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* WeaponLeft;

	bool bIsHitPlayer = false;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	FName TraceStartSocketL = TEXT("FX_Trail_02_L");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	FName TraceEndSocketL = TEXT("FX_Trail_01_L");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	FName TraceStartSocketR = TEXT("FX_Trail_02_R");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	FName TraceEndSocketR = TEXT("FX_Trail_01_R");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	float TraceRadius = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_GameTraceChannel3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	bool bDrawDebugTrace = true;


	UPROPERTY()
	FVector PrevStart = FVector::ZeroVector;

	UPROPERTY()
	FVector PrevEnd = FVector::ZeroVector;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActors;
};
