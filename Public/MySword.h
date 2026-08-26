// 武器の当たり判定と属性攻撃を処理する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MySword.generated.h"

class AEnemyOrigin;
class AMyPlayer;
class UEffectSubsystem;
class UGameAudioSubsystem;
class UPlayerAttributeSet;

// 武器の当たり判定と属性攻撃を処理する役割を持つ。
UCLASS()
class PJ26_API AMySword : public AActor
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	AMySword();

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;
	// 構成要素の生成完了後に、相互参照とイベントを接続する。
	virtual void PostInitializeComponents() override;

public:

	// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
	virtual void Tick(float DeltaTime) override;

	// 重なりを検知した相手の種類を確認し、該当するダメージまたは取得効果を適用する。
	UFUNCTION(BlueprintNativeEvent, Category = "Hit")
	void OnOverlapsBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	// 重なり終了の通知を受け取り、必要な接触状態を解除する。
	UFUNCTION(BlueprintNativeEvent, Category = "Hit")
	void OnOverlapsEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// 武器衝突を有効にし、後続処理を受け付ける状態にする。
	void EnableWeaponCollision();
	// 武器衝突を無効にし、不要な更新や判定を停止する。
	void DisableWeaponCollision();

public:
	// 攻撃開始時に武器の衝突判定を有効にし、直前のソケット位置を記録する。
	void StartTraceAttack();
	// 攻撃終了時に武器の衝突判定を停止し、命中対象の記録を初期化する。
	void EndTraceAttack();
	// 前回と現在の武器位置の間をスイープし、高速な攻撃の衝突を検出する。
	void PerformWeaponTrace();

	void ApplyAtkAttribute(AMyPlayer* Player, AEnemyOrigin* Enemy,
		UEffectSubsystem* EffectSubsystem, UGameAudioSubsystem* AudioSubsystem);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> SwordMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	FName TraceStartSocket = TEXT("Root");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	FName TraceEndSocket = TEXT("End");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	float TraceRadius = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_GameTraceChannel2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace", meta = (AllowPrivateAccess = "true"))
	bool bDrawDebugTrace = true;

	UPROPERTY()
	bool IsTraceAttackActive = false;

	UPROPERTY()
	FVector PrevStart = FVector::ZeroVector;

	UPROPERTY()
	FVector PrevEnd = FVector::ZeroVector;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActors;

	bool bIsHitEnemy = false;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UTexture2D* FireIconTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UTexture2D* ShockIconTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UTexture2D* WindIconTexture;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int32 MaxHitSoundCnt = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int32 StateDuration = 10;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CameraShake", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraShakeBase> AtkCameraShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CameraShake", meta = (AllowPrivateAccess = "true"))
	float HitStopThreshhold = 0.0002f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CameraShake", meta = (AllowPrivateAccess = "true"))
	float CamerashakeScale = 0.1f;
};
