// ロックオン対象の検索と追従を処理する。

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraLockOnComponent.generated.h"


class UCameraComponent;
class AMyPlayer;
class ULockonWidget;

// ロックオン対象の検索と追従を処理する役割を持つ。
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PJ26_API UCameraLockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	UCameraLockOnComponent();

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;

public:
	// 周囲の候補を画面正面との角度で比較し、最適な対象をロックオン対象に設定する。
	void FindBestTarget();
	// 現在の対象を基準に左右の候補を比較し、指定方向の対象へ切り替える。
	void FindSideTarget(bool bRight);
	// 対象の存在・距離・生存状態・遮蔽物を確認し、追跡可能か判定する。
	bool IsTargetValid(AActor* Target) const;

	// 対象の有効性を確認し、照準表示とキャラクターの追従状態を更新する。
	void UpdateLockOn(float DeltaTime);
	// キャラクターとカメラを対象方向へ滑らかに補間回転させる。
	void FaceLockOnTarget(float DeltaTime);

	// カメラが有効ならカメラ位置を、無効ならプレイヤー位置を返す。
	FVector GetLockOnTargetLocation() const;
	// カメラが有効ならカメラ前方を、無効ならプレイヤー前方を返す。
	FVector GetLockOnTargetForward() const;

	// ロックオンで使用するカメラを設定する。
	void SetCamera(UCameraComponent* InCamera);
	// ロックオンを使用するプレイヤーを設定する。
	void SetOwnerPlayer(AMyPlayer* InPlayer);

	// 対象を取得し、呼び出し元へ返す。
	const AActor* GetLockedTarget() { return LockedTarget; }
	// 対象参照を解除し、回転設定と照準表示を通常状態へ戻す。
	void ClearLockOn();
public:

	// 毎フレーム、構成要素が担当する追従処理と状態確認を行う。
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<ULockonWidget> PlayerLockonWidgetClass;

	UPROPERTY()
	TObjectPtr<ULockonWidget> PlayerLockonWidgetInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LockOn")
	float LockOnRadius = 2500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LockOn")
	float MinDot = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LockOn")
	float LoseDistance = 1800.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LockOn")
	float CharacterRotateSpeed = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LockOn")
	float CameraRotateSpeed = 12.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LockOn")
	TSubclassOf<AActor> LockOnTargetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LockOn")
	TEnumAsByte<ECollisionChannel> VisibilityChannel = ECC_Visibility;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	bool IsLockOn = false;

private:
	AMyPlayer* OwnerPlayer;
	AActor* LockedTarget;

private:
	class UCameraComponent* Camera;
};
