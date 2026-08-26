// 画面表示と操作を処理する。

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHpWidget.generated.h"

class UProgressBar;
class UImage;
class AMyPlayer;


// 画面表示と操作を処理する役割を持つ。
UCLASS()
class PJ26_API UPlayerHpWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 画面部品の生成後に、子要素・イベント・初期表示を設定する。
	virtual void NativeConstruct() override;

	// ロックオンを使用するプレイヤーを設定する。
	void SetOwnerPlayer(AMyPlayer* InPlayer);
	// 体力を現在の実行状態に合わせて再計算し、関連要素へ反映する。
	void UpdateHp();
	// スタミナを現在の実行状態に合わせて再計算し、関連要素へ反映する。
	void UpdateSp();
	// 攻撃属性を現在の実行状態に合わせて再計算し、関連要素へ反映する。
	void UpdateAttackAttributeImage();

	// ロックオンを現在の実行状態に合わせて再計算し、関連要素へ反映する。
	void UpdateLockOnMarker(AActor* TargetActor);
	// ロックオンを非表示にし、入力や更新の対象から外す。
	void HideLockOnMarker();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> SPBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> AttackAttrImage;

	UPROPERTY()
	TObjectPtr<AMyPlayer> OwnerPlayer;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icons")
	TObjectPtr<UTexture2D> DefaultAttrIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icons")
	TObjectPtr<UTexture2D> FireAttrIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icons")
	TObjectPtr<UTexture2D> LightningAttrIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icons")
	TObjectPtr<UTexture2D> WindAttrIcon;
};
