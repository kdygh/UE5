// 武器の軌跡演出を処理する。

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <pj26/MyPlayer.h>
#include "WeaponTrailComponent.generated.h"


class UNiagaraSystem;
class USkeletalMeshComponent;

// 武器の軌跡演出を処理する役割を持つ。
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PJ26_API UWeaponTrailComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	UWeaponTrailComponent();

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;

public:
	// 現在武器軌跡システムを取得し、呼び出し元へ返す。
	UNiagaraSystem* GetCurrentWeaponTrailSystem(EAttackAttribute CurrentAttribute) const;
	// 攻撃属性に応じた武器軌跡を開始する。
	void StartWeaponTrail(USkeletalMeshComponent* Mesh, EAttackAttribute CurrentAttribute);
	// 武器軌跡を終了し、関連状態とタイマーを解除する。
	void StopWeaponTrail();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponTrail", meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* DefaultWeaponTrail;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponTrail", meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* WindWeaponTrail;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponTrail", meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* FireWeaponTrail;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponTrail", meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* LightningWeaponTrail;

private:
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ActiveWeaponTrail;
};
