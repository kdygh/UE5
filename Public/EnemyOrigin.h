// 敵に共通する状態と戦闘を処理する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/WidgetComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Animation/AnimMontage.h"
#include "Sound/SoundBase.h"
#include "EnemyAI/NormalEnemyAIController.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet/EnemyAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "UI/Enemy/EnemyHPWidget.h"
#include "UI/Enemy/BossStatusWidget.h"
#include <NiagaraComponent.h>
#include "EnemyOrigin.generated.h"

// 敵に共通する状態と戦闘を処理する役割を持つ。
UENUM()
enum class ECharacterState
{
	Normal,
	Fire,
	Lightning,
	Wind,
};

// 敵に共通する状態と戦闘を処理する役割を持つ。
USTRUCT()
struct FPrepareAtkTaskMemory
{
	GENERATED_BODY()

	float EndTime = 0.f;
	float NextMoveTime = 0.f;
};

struct FOnAttributeChangeData;
struct FGameplayTagContainer;
struct FActiveDebuffInfo;
struct FStatusBuildUpInfo;

class UMaterialInstanceDynamic;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDiedSignature, AEnemyOrigin*, DeadEnemy);

// 敵に共通する状態と戦闘を処理する役割を持つ。
UCLASS()
class PJ26_API AEnemyOrigin : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	AEnemyOrigin();

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	// 値の変更通知を受け取り、内部状態と関連表示へ反映する。
	void OnHpChanged(const FOnAttributeChangeData& Data);
	// 値の変更通知を受け取り、内部状態と関連表示へ反映する。
	void OnDefChanged(const FOnAttributeChangeData& Data);
	// 値の変更通知を受け取り、内部状態と関連表示へ反映する。
	void OnMoveSpeedChanged(const FOnAttributeChangeData& Data);
	// 現在の体力割合を計算し、頭上の体力表示とボス状態画面へ反映する。
	void UpdateHpWidget();

public:

	// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
	virtual void Tick(float DeltaTime) override;


	// 移動・視点・戦闘に使用する入力と処理関数を関連付ける。
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 能力システム構成要素を取得し、呼び出し元へ返す。
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(BlueprintAssignable, Category = "Enemy|Event")
	FOnEnemyDiedSignature OnEnemyDeath;

	// 登録済みの状態異常効果と変更イベントをすべて解除し、一覧を空にする。
	void RemoveAllDebuffs();

public:
	// 派生クラスで攻撃開始時の状態と衝突判定を設定するための入口を定義する。
	virtual void attack_start() {}
	// 派生クラスで攻撃終了時の状態と衝突判定を解除するための入口を定義する。
	virtual void attack_end() {}

	// 衝突相手とダメージ量を確認し、プレイヤーへ通常の衝突ダメージを適用する。
	virtual void CalCollisionDamage(AActor* OtherActor) {}
	// 衝突相手とダメージ量を確認し、プレイヤーへ通常の衝突ダメージを適用する。
	virtual void CalCollisionDamage(float DamageAmount) {}
	// 衝突相手とダメージ量を確認し、プレイヤーへ通常の衝突ダメージを適用する。
	virtual void CalCollisionDamage(AActor* OtherActor, const FVector& emitPos) {}
	// 被弾状態へ切り替え、攻撃中断・アニメーション・関連演出を処理する。
	virtual void Damaged() {}

	// 衝突相手を確認し、プレイヤーへ属性またはスタミナに関するダメージを適用する。
	virtual void CalCollisionDamageSP(float DamageAmount) {}

	// 気絶状態へ切り替え、現在の行動を停止して気絶演出を開始する。
	virtual void Stun() {}

	// 画面表示を現在とは別の状態または候補へ切り替える。
	virtual void ToggleUI() {}

	// 体力を現在の実行状態に合わせて再計算し、関連要素へ反映する。
	virtual void UpdateHealth(float changedHP) {}

	// 制御装置を指定された値へ更新する。
	virtual void SetAIController(ANormalEnemyAIController* _controller) {}
	// 現在は使用していない処理。
	// 制御装置を指定された値へ更新する。
	//virtual void SetAIController(ABossAIController* _controller) {}
	// プレイヤーに関するイベントを受け取り、関連状態を更新する。
	virtual void OnFollowPlayer(bool _isPlayerInSight) {}

	// 現在の戦闘状態と入力条件を確認し、対応する攻撃処理を実行する。
	virtual void MeleeAttack() {}
	// 現在は使用していない処理。
	// 現在の戦闘状態と入力条件を確認し、対応する攻撃処理を実行する。
	//virtual void MeleeAttack(int atknum = 0) {}

	// 敵の視認距離・視野角・検知対象を設定して知覚機能を準備する。
	virtual void setup_perception_system(ANormalEnemyAIController* _controller) {}
	// 知覚した対象の情報を確認し、追跡状態をブラックボードへ反映する。
	virtual void on_target_detected(AActor* actor, FAIStimulus const stimulus) {}

	// ダメージを指定された値へ更新する。
	virtual void SetCanGiveDamage(bool _canGiveDamage) { CanGiveDamage = _canGiveDamage; }
	// ダメージを取得し、呼び出し元へ返す。
	virtual bool GetCanGiveDamage() { return CanGiveDamage; }

	// ダメージを指定された値へ更新する。
	virtual void SetCanTakeDamage(bool _canTakeDamage) { CanTakeDamage = _canTakeDamage; }
	// ダメージを取得し、呼び出し元へ返す。
	virtual bool GetCanTakeDamage() { return CanTakeDamage; }

	// 対象を取得し、呼び出し元へ返す。
	virtual bool GetIsStunned() { return IsStunned; }

	// 攻撃範囲を取得し、呼び出し元へ返す。
	virtual float GetAtkRange() { return AtkRange; }
	// アニメーション通知名に応じて、攻撃判定・回避・パリィ・死亡状態を切り替える。
	virtual void OnAnimAction(FName ActionName) {}

	// 対象を取得し、呼び出し元へ返す。
	virtual bool GetDetectEnemies() { return bDetectEnemies; }
	// 対象を取得し、呼び出し元へ返す。
	virtual bool GetDetectFriendlies() { return bDetectFriendlies; }
	// 対象を取得し、呼び出し元へ返す。
	virtual bool GetDetectNeutrals() { return bDetectNeutrals; }

	// 処理の完了を確認し、後続の状態または画面へ移行する。
	virtual bool HasMontageFinished() { return false; }

	// 視界を取得し、呼び出し元へ返す。
	virtual UAISenseConfig_Sight* GetSenseConfigSight() { return m_sightConfig; }
	// 知覚を取得し、呼び出し元へ返す。
	virtual UAIPerceptionComponent* GetPerceptionComp() { return m_aiPercepComp; }

	// アニメーションモンタージュを取得し、呼び出し元へ返す。
	virtual UAnimMontage* GetAnimMontage() { return m_animMontage; }

	// ラグドール移行後にアニメーション更新を停止し、物理姿勢だけを維持する。
	virtual void DisableAnimationAfterRagdoll();

	// 対象を取得し、呼び出し元へ返す。
	virtual bool GetIsAttacking() { return IsAttacking; }
	// 対象を指定された値へ更新する。
	virtual void SetIsAttacking(bool _isAttacking) { IsAttacking = _isAttacking; }

	// 命中を取得し、呼び出し元へ返す。
	virtual bool GetIsHitByObject() { return bIsHitByObject; }
	virtual void SetIsHitByObject(bool _isHitByObject)
	{
		bIsHitByObject = _isHitByObject;
	}
public:
	// 生存状態を解除し、移動・衝突・戦闘を停止した後に死亡演出を開始する。
	virtual void Death() {}
	// キャラクターが現在生存しているか判定して返す。
	virtual bool GetIsAlive() { return IsAlive; }
	// オンプレイヤーを指定された値へ更新する。
	virtual void SetLockedOnByPlayer(bool _isLockedOnbyPlayer) { isLockedOnByPlayer = _isLockedOnbyPlayer; }
	// オンプレイヤーを取得し、呼び出し元へ返す。
	virtual bool GetLockedOnByPlayer() { return isLockedOnByPlayer; }
	// 対象を解除し、再利用可能な初期状態へ戻す。
	virtual void ResetSetting() {}
	// ロックオン地点を指定された値へ更新する。
	virtual void SetLockOnPointVisible(bool _visible) {}
	// 対象とプレイヤーの時間倍率を下げ、タイマーで通常速度への復帰を予約する。
	virtual void DoHitStop(float Duration) {}
	// 一時的に変更した時間倍率を通常の値へ戻す。
	virtual void ResetTimeDilation(float Duration) {}

	// 属性を取得し、呼び出し元へ返す。
	virtual UEnemyAttributeSet* GetAttributeSet() const { return AttributeSet; }

	// 体力割合を取得し、呼び出し元へ返す。
	virtual float GetHpPercent() const { return 0.0f; }
	// スタミナ割合を取得し、呼び出し元へ返す。
	virtual float GetSpPercent() const { return 0.0f; }
	// 体力を指定された値へ更新する。
	virtual void SetHP(float NewHP) {}
	// スタミナを指定された値へ更新する。
	virtual void SetSP(float NewSP) {}

	virtual void SetCharacterState(ECharacterState State)
	{
		CharacterState = State;
	}

	// 対象を取得し、呼び出し元へ返す。
	virtual ECharacterState GetCharacterState() { return CharacterState; }

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PawnSensing)
	bool isPlayerInSight;

protected:
	float MaxHP;
	float CurrHP;
	float AtkPower;

	float AtkRange;

	bool bDetectEnemies;
	bool bDetectFriendlies;
	bool bDetectNeutrals;

	bool CanGiveDamage = true;
	bool CanTakeDamage = true;

	bool IsAttacking = false;

	bool IsStunned = false;

	bool bIsHitByObject = false;
	bool bStatsInitialized = false;

	float BaseMoveSpeed;

	UPROPERTY(BlueprintReadOnly)
	bool IsAlive;

	ECharacterState CharacterState;

	UPROPERTY()
	FTimerHandle UIToggleTimer;

	bool isLockedOnByPlayer;

	UWidgetComponent* m_widgetComp;
	UWidgetComponent* m_widgetLockOnComp;

	UParticleSystem* m_particle;
	UParticleSystem* m_particle2;
	UParticleSystem* m_particle3;

	FGameplayTagContainer EnemyTags;

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Anim", meta = (AllowPrivateAccess = "true"))
	UAISenseConfig_Sight* m_sightConfig;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Anim", meta = (AllowPrivateAccess = "true"))
	UAIPerceptionComponent* m_aiPercepComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* m_animMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	USoundBase* m_soundDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> EnemyInfoTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEnemyAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	float LightningThreshold = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	float FireThreshold = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	float WindThreshold = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> HpWidgetComponent;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UBossStatusWidget> BossHpWidgetClass;

	UPROPERTY()
	TObjectPtr<UBossStatusWidget> BossHpWidgetInstance;

public:
	// 有効デバフを取得し、呼び出し元へ返す。
	const TArray<FActiveDebuffInfo>& GetActiveDebuffs() const;
	// 有効状態蓄積を取得し、呼び出し元へ返す。
	const TArray<FStatusBuildUpInfo>& GetActiveStatusBuildUps() const;

	void AddOrRefreshDebuff(
		const FGameplayTag& DebuffTag,
		float CurrDuration,
		float WholeDuration,
		ECharacterState State,
		UTexture2D* IconTexture
	);

	// 指定タグと一致する状態異常効果および表示情報を削除する。
	void RemoveDebuffByTag(const FGameplayTag& DebuffTag);

	// 雷を指定された値へ更新する。
	void SetCurrLightningThreshold(float NewThreshold) { CurrLightningThreshold = NewThreshold; }
	// 炎を指定された値へ更新する。
	void SetCurrFireThreshold(float NewThreshold) { CurrFireThreshold = NewThreshold; }
	// 風を指定された値へ更新する。
	void SetCurrWindThreshold(float NewThreshold) { CurrWindThreshold = NewThreshold; }

	// 雷を管理対象へ追加し、関連状態を更新する。
	void AddLightningThreshold(float NewThreshold) { CurrLightningThreshold += NewThreshold; }
	// 炎を管理対象へ追加し、関連状態を更新する。
	void AddFireThreshold(float NewThreshold) { CurrFireThreshold += NewThreshold; }
	// 風を管理対象へ追加し、関連状態を更新する。
	void AddWindThreshold(float NewThreshold) { CurrWindThreshold += NewThreshold; }

	// 雷を取得し、呼び出し元へ返す。
	float GetCurrLightningThreshold() const { return CurrLightningThreshold; }
	// 炎を取得し、呼び出し元へ返す。
	float GetCurrFireThreshold() const { return CurrFireThreshold; }
	// 風を取得し、呼び出し元へ返す。
	float GetCurrWindThreshold() const { return CurrWindThreshold; }

	// 雷を取得し、呼び出し元へ返す。
	float GetLightningThreshold() const { return LightningThreshold; }
	// 炎を取得し、呼び出し元へ返す。
	float GetFireThreshold() const { return FireThreshold; }
	// 風を取得し、呼び出し元へ返す。
	float GetWindThreshold() const { return WindThreshold; }

protected:
	UPROPERTY()
	TArray<FActiveDebuffInfo> ActiveDebuffs;

	UPROPERTY()
	TArray<FStatusBuildUpInfo> ActiveStatusBuildUps;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> WindStatusFXComponent;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> FireStatusFXComponent;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> LightningStatusFXComponent;

protected:
	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> CharacterMIDs;

	UPROPERTY(VisibleInstanceOnly, Category = "Status Rim")
	bool IsStatusRimActive = false;

	UPROPERTY(EditAnywhere, Category = "Status Rim")
	float RimMinIntensity = 1.f;

	UPROPERTY(EditAnywhere, Category = "Status Rim")
	float RimMaxIntensity = 6.f;

	UPROPERTY(EditAnywhere, Category = "Status Rim")
	float RimPulseSpeed = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Rim", meta = (AllowPrivateAccess = "true"))
	UMaterial* RimLightMaterial;

	float RimElapsedTime = 0.f;

	UPROPERTY()
	bool IsTraceAttackActive = false;

	UPROPERTY()
	bool IsTraceAttackActiveL = false;

	UPROPERTY()
	bool IsTraceAttackActiveR = false;

protected:
	// キャラクターメッシュの動的マテリアルを生成し、元の色と発光値を保存する。
	void InitializeCharacterMaterials();

	// 輪郭弱を取得し、呼び出し元へ返す。
	FLinearColor GetRimLightColor();

	void StartStatusRimLight(
		const FLinearColor& RimColor,
		float Duration
	);

	// 有効な状態異常を確認し、表示する輪郭発光の状態を決定する。
	void AdjustStatusRimLight();

	// 残り時間に応じて、輪郭発光の強度と色を毎フレーム更新する。
	void UpdateStatusRimLight(float DeltaTime);

private:
	float CurrLightningThreshold = 0.f;

	float CurrFireThreshold = 0.f;

	float CurrWindThreshold = 0.f;

	FTimerHandle StatusRimTimerHandle;
};
