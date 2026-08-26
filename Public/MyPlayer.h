// プレイヤーの入力と戦闘を処理する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyAnimInstancePj26.h"
#include "AbilitySystemInterface.h"
#include "MyPlayer.generated.h"

// プレイヤーの入力と戦闘を処理する役割を持つ。
UENUM()
enum class EATK
{
	LIGHTATK1 = 1 UMETA(DisplayName = "LIGHTATK1"),
	LIGHTATK2 UMETA(DisplayName = "LIGHTATK2"),
	LIGHTATK3 UMETA(DisplayName = "LIGHTATK3"),
	LIGHTATK4 UMETA(DisplayName = "LIGHTATK4"),
	LIGHTATKMAX UMETA(DisplayName = "LIGHTATKMAX"),

	HEAVYATK1 = 6 UMETA(DisplayName = "HEAVYATK1"),
	HEAVYATK2 UMETA(DisplayName = "HEAVYATK2"),
	HEAVYATK3 UMETA(DisplayName = "HEAVYATK3"),
	HEAVYATKMAX UMETA(DisplayName = "HEAVYATKMAX"),

	COMB11 = 11 UMETA(DisplayName = "COMBO11"),
	COMB1MAX UMETA(DisplayName = "COMBO1MAX"),

	COMB21 = 21 UMETA(DisplayName = "COMBO21"),
	COMB22 UMETA(DisplayName = "COMBO22"),
	COMB23 UMETA(DisplayName = "COMBO23"),
	COMB2MAX UMETA(DisplayName = "COMBO2MAX"),

	COMB31 = 31 UMETA(DisplayName = "COMBO31"),
	COMB32 UMETA(DisplayName = "COMBO32"),
	COMB3MAX UMETA(DisplayName = "COMBO3MAX"),

	COMB41 = 41 UMETA(DisplayName = "COMBO41"),
	COMB42 UMETA(DisplayName = "COMBO42"),
	COMB4MAX UMETA(DisplayName = "COMBO4MAX")
};

// プレイヤーの入力と戦闘を処理する役割を持つ。
UENUM(BlueprintType)
enum class EAttackAttribute : uint8
{
	None,
	Fire,
	Lightning,
	Wind
};

class AMyGhostActor;
class UNiagaraSystem;
class UNiagaraComponent;
class ALightningActor;
class AShockwaveActor;
class UPlayerHpWidget;
class ULockonWidget;
class UAbilitySystemComponent;
class UPlayerAttributeSet;
class UGameplayEffect;
class AMySword;
class UItemNotificationPanel;
class UPlayerAtkPowerNotifyPanel;
class AEnemyOrigin;

// プレイヤーの入力と戦闘を処理する役割を持つ。
UCLASS()
class PJ26_API AMyPlayer : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	AMyPlayer();

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;
	// 構成要素の生成完了後に、相互参照とイベントを接続する。
	virtual void PostInitializeComponents() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
	virtual void Tick(float DeltaTime) override;


	// 移動・視点・戦闘に使用する入力と処理関数を関連付ける。
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// カメラ方向を基準に、前後入力をワールド上の移動方向へ変換する。
	void MoveForward(float Value);
	// カメラ方向を基準に、左右入力をワールド上の移動方向へ変換する。
	void MoveRight(float Value);
	// 左右の視点入力を回転へ反映する。
	void Turn(float Value);
	// 上下の視点入力を回転へ反映する。
	void LookUp(float Value);
	// スティック方向と再入力可能状態を確認し、左右の対象切り替えを一度だけ実行する。
	void SwitchLockOnInput(float Value);

public:
	// 現在の戦闘状態を確認した後、弱攻撃を入力待ち行列へ登録する。
	void Atk_Normal();
	// 現在の戦闘状態を確認した後、強攻撃を入力待ち行列へ登録する。
	void Atk_Strong();
	// 現在の連続攻撃段階と入力を基に、次の攻撃を待ち行列へ登録する。
	void Atk_Combo();
	// 攻撃アニメーションの受付区間で、次の連続攻撃と先行入力を許可する。
	void EnableNextCombo();
	// 現在の入力方向と行動状態を確認し、回避移動と無敵状態を開始する。
	void Evade();
	// 実行可能状態を確認した後、パリィアニメーションと衝突判定を開始する。
	void Parry();
	// 水平方向と指定された強さを使用し、キャラクターを押し戻して行動を制限する。
	void Knockback(const FVector& Direction, const FRotator& Rotation, float Power);
	// アニメーション通知名に応じて、攻撃判定・回避・パリィ・死亡状態を切り替える。
	void OnAnimAction(FName ActionName);
	// 攻撃名に対応するアニメーション区間・攻撃属性・連続攻撃状態を設定する。
	void OnAtk(FName AtkName);
	// 衝突相手とダメージ量を確認し、プレイヤーへ通常の衝突ダメージを適用する。
	void CalCollisionDamage(float DamageAmount, AActor* OtherActor);
	// 生存状態を解除し、移動・衝突・戦闘を停止した後に死亡演出を開始する。
	void Death();
	// 死亡演出用カメラを生成し、プレイヤーの視点を死亡演出へ切り替える。
	void StartDeathCamera();
	// プレイヤーのギブアップ入力を受け取り、通常の死亡処理へ移行する。
	void GiveUp();

	// ラグドール移行後にアニメーション更新を停止し、物理姿勢だけを維持する。
	void DisableAnimationAfterRagdoll();
	// 能力システムを通して、指定したダメージ量を対象の体力へ適用する。
	void ApplyDamageToTarget(float DamageAmount, AActor* TargetActor);

public:
	// 雷効果の実行条件を確認し、対象へ効果を適用する。
	void ApplyLightningEffect(AActor* TargetActor);
	// 風効果の実行条件を確認し、対象へ効果を適用する。
	void ApplyWindEffect(AActor* TargetActor);
	// 炎効果の実行条件を確認し、対象へ効果を適用する。
	void ApplyFireEffect(AActor* TargetActor);
	// 指定されたゲーム効果を生成し、対象の能力システムへ適用する。
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> EffectClass);
	// 属性攻撃に必要なスタミナを消費し、自動回復を一定時間停止する。
	void ConsumeStamina(float Cost);
	// 攻撃スタミナを取得し、呼び出し元へ返す。
	float GetElementAttackStaminaCost(EAttackAttribute Attr) const;
	// 既存のタイマーを更新し、指定時間だけスタミナの自動回復を停止する。
	void BlockStaminaRegen(float Delay);
	// 回復待機時間の終了後、スタミナの自動回復を再び許可する。
	void EnableStaminaRegen();
	// 回復待機状態と最大値を確認し、経過時間に応じてスタミナを回復する。
	void RegenerateStamina(float DeltaTime);
	// 現在のスタミナで属性攻撃を実行できるか判定して返す。
	bool GetIsStaminaEnough() const { return IsStaminaEnough; }

public:
	// 弱攻撃を取得し、呼び出し元へ返す。
	UINT GetLightAtkCnt() const { return LightAtkCnt; }
	// 強攻撃を取得し、呼び出し元へ返す。
	UINT GetHeavyAtkCnt() const{ return HeavyAtkCnt; }
	// 連続攻撃を取得し、呼び出し元へ返す。
	UINT GetComboCnt() const { return ComboCnt; }

public:
	// 回避状態を取得し、呼び出し元へ返す。
	bool GetIsEvading() const { return IsEvading; }
	// パリィ状態を取得し、呼び出し元へ返す。
	bool GetIsParrying() const;
	// パリィ成功状態・ヒットストップ・効果音・視覚効果をまとめて適用する。
	void SetParrying(FVector FXLocation);

	FName GetAtkQueue()
	{
		if (AtkQueue.IsEmpty())
		{
			return "NULL";
		}

		FName* temp = AtkQueue.Peek();

		AtkQueue.Dequeue(*temp);

		return *temp;
	}

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	UChildActorComponent* SwordComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	UAnimMontage* HeavyAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	UAnimMontage* ComboAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Evade")
	UAnimMontage* EvadeMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parry")
	UAnimMontage* ParryMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death")
	UAnimMontage* DeathAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	UMaterialInterface* GhostMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DashPawer")
	double DashPower = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DashGhost")
	TSubclassOf<AMyGhostActor> GhostClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DashGhostTrail")
	UNiagaraComponent* GhostTrailEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TSubclassOf<AShockwaveActor> ShockwaveActorClass;

public:
	// 現在の状態に応じて最適対象の取得またはロックオン解除を実行する。
	UFUNCTION(BlueprintCallable)
	void ToggleLockOn();

	// 現在の対象より画面左側にいる候補を検索し、最適な対象へ切り替える。
	UFUNCTION(BlueprintCallable)
	void SwitchLockOnLeft();

	// 現在の対象より画面右側にいる候補を検索し、最適な対象へ切り替える。
	UFUNCTION(BlueprintCallable)
	void SwitchLockOnRight();

	// 対象参照を解除し、回転設定と照準表示を通常状態へ戻す。
	UFUNCTION(BlueprintCallable)
	void ClearLockOn();

public:
	// 体力割合を取得し、呼び出し元へ返す。
	float GetHpPercent() const;
	// スタミナ割合を取得し、呼び出し元へ返す。
	float GetSpPercent() const;

	// 体力を指定された値へ更新する。
	void SetHP(float NewHP);
	// スタミナを指定された値へ更新する。
	void SetSP(float NewSP);
	// 属性を取得し、呼び出し元へ返す。
	UPlayerAttributeSet* GetAttributeSet() const { return AttributeSet; }
	// 能力システム構成要素を取得し、呼び出し元へ返す。
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComp; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UPlayerHpWidget> PlayerHpWidgetClass;

	UPROPERTY()
	TObjectPtr<UPlayerHpWidget> PlayerHpWidgetInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<ULockonWidget> PlayerLockonWidgetClass;

	UPROPERTY()
	TObjectPtr<ULockonWidget> PlayerLockonWidgetInstance;

public:
	// プレイヤーのロックオン状態を指定された値へ更新する。
	void SetIsLockingOn(bool isLockingOn) { IsLockingOn = isLockingOn; }
	// プレイヤーが現在ロックオン中か判定して返す。
	bool GetIsLockingOn() const { return IsLockingOn; }
	// キャラクターが現在生存しているか判定して返す。
	bool GetIsAlive() const { return IsAlive; }

public:
	// 体力を上限の範囲内で指定量だけ回復する。
	void RecoverHP(float Amount);
	// プレイヤー最大体力を指定量だけ増加させ、関連表示へ反映する。
	void IncreasePlayerMaxHP(float Amount);
	// プレイヤー最大スタミナを指定量だけ増加させ、関連表示へ反映する。
	void IncreasePlayerMaxSP(float Amount);
	// プレイヤー攻撃威力を指定量だけ増加させ、関連表示へ反映する。
	void IncreasePlayerAtkPower(float Amount);
	// プレイヤー防御力を指定量だけ増加させ、関連表示へ反映する。
	void IncreasePlayerDef(float Amount);
	// プレイヤー速度を指定量だけ増加させ、関連表示へ反映する。
	void IncreasePlayerSpeed(float Amount);
	// 取得したアイテムの説明文を通知画面へ追加して表示する。
	void ShowItemDesc(const FText message);
	// 現在攻撃属性を指定された値へ更新する。
	void SetCurrentAttackAttribute();

	EAttackAttribute GetCurrentAttackAttribute() const
	{
		return CurrentAttackAttribute;
	}

	// 雷スタミナを取得し、呼び出し元へ返す。
	float GetLightningSP() const { return LightningSP; }
	// 炎スタミナを取得し、呼び出し元へ返す。
	float GetFireSP() const { return FireSP; }
	// 風スタミナを取得し、呼び出し元へ返す。
	float GetWindSP() const { return WindSP; }

public:
	// 攻撃属性に応じた武器軌跡を開始する。
	void StartWeaponTrail();

public:
	// ゲームを一時停止し、入力方式と一時停止画面の表示状態を切り替える。
	void Pause();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> PlayerInfoTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPlayerAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	EAttackAttribute CurrentAttackAttribute = EAttackAttribute::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Effects", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Effects", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> ShockEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Effects", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> FireEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Effects", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> WindEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Effects", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> ConsumeStaminaEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Effects", meta = (AllowPrivateAccess = "true"))
	float BlockStaminaRegenDelay = 0.5f;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	int32 PlayerMaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	int32 PlayerMaxSP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	float PlayerAtkPower;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	float PlayerDef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	float PlayerSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	float PlayerSPGenRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	float LightningSP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	float FireSP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	float WindSP;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CameraShake", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraShakeBase> CameraShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CameraShake", meta = (AllowPrivateAccess = "true"))
	float HitStopThreshhold = 0.0003f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CameraShake", meta = (AllowPrivateAccess = "true"))
	float CamerashakeScale = 0.2f;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UItemNotificationPanel> ItemNotificationPanelClass;

	UPROPERTY()
	TObjectPtr<UItemNotificationPanel> ItemNotificationPanel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UPlayerAtkPowerNotifyPanel> PlayerAtkPowerNotifyPanelClass;

	UPROPERTY()
	TObjectPtr<UPlayerAtkPowerNotifyPanel> PlayerAtkPowerNotifyPanel;

private:
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ActiveWeaponTrail;

private:
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
	class UCameraLockOnComponent* LockOnComponent;

	UPROPERTY(VisibleAnywhere)
	class UWeaponTrailComponent* WeaponTrailComponent;

	UPROPERTY(VisibleAnywhere)
	class UMyAnimInstancePj26* AnimInst;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parry", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* CapsuleParry;

private:
	UINT LightAtkCnt = 0;
	UINT HeavyAtkCnt = 5;
	UINT ComboCnt = 0;

	UINT RootMotionMode = 3;

	float ForwardInput = 0.f;
	float RightInput = 0.f;
	float TurnValue = 0.0f;

	bool canCombo = false;
	bool IsEvading = false;
	bool IsParrying = false;
	bool IsLockingOn = false;
	bool IsAlive = true;
	bool CanRegenStamina = true;
	bool IsStaminaEnough = true;
	bool isLockOnSwitchTriggered = false;

	FTimerHandle StaminaRegenDelayHandle;

	TQueue<FName> AtkQueue;

	// 現在の連続攻撃段階と強攻撃入力を基に、次の攻撃を決定する。
	void DetermineNextAtk(UINT _comboCnt, bool _isHeavyAtk);
	// 回避状態を有効にし、後続処理を受け付ける状態にする。
	void EnableExtraEvading();
	// パリィ状態を有効にし、後続処理を受け付ける状態にする。
	void EnableExtraParrying();
	// 死亡・被弾・回避・パリィ状態を確認し、新しい攻撃を開始できるか判定する。
	bool AtkCheck();
	// 再生中の攻撃と武器判定を停止し、攻撃に関する状態を初期化する。
	void StopAtk();

	// 現在入力方向を取得し、呼び出し元へ返す。
	FVector GetCurrentInputDirection() const;
	// 回避中のプレイヤー姿勢を使用し、残像アクターを現在位置へ生成する。
	void SpawnAfterImage();
};
