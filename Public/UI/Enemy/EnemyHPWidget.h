// 敵固有の行動と戦闘を処理する。

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "EnemyHPWidget.generated.h"


class UProgressBar;
class UWrapBox;
class AEnemyOrigin;
class UDebuffIconWidget;
class UStatusBuildUpIcon;
class UTexture2D;


// 敵固有の行動と戦闘を処理する役割を持つ。
UCLASS()
class PJ26_API UEnemyHPWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 画面部品の生成後に、子要素・イベント・初期表示を設定する。
	virtual void NativeConstruct() override;
	// 画面部品の破棄前に、外部イベントと保持参照を解除する。
	virtual void NativeDestruct() override;

	// 所有者敵を指定された値へ更新する。
	void SetOwnerEnemy(AEnemyOrigin* InEnemy);
	// 体力を現在の実行状態に合わせて再計算し、関連要素へ反映する。
	void UpdateHp();
	// スタミナを現在の実行状態に合わせて再計算し、関連要素へ反映する。
	void UpdateSp();

	// 有効な状態異常情報を取得し、アイコンの追加・更新・削除を画面へ反映する。
	void RefreshDebuffs();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> SPBar;

	UPROPERTY()
	TObjectPtr<AEnemyOrigin> OwnerEnemy;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWrapBox> DebuffWrapBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debuff")
	TSubclassOf<UDebuffIconWidget> DebuffIconWidgetClass;

private:
	FTimerHandle DebuffRefreshTimerHandle;
};
