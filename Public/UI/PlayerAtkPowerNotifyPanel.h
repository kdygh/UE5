// 画面表示と操作を処理する。

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerAtkPowerNotifyPanel.generated.h"


class UPlayerAtkPowerNotificationEntry;
class UVerticalBox;

// 画面表示と操作を処理する役割を持つ。
UCLASS()
class PJ26_API UPlayerAtkPowerNotifyPanel : public UUserWidget
{
	GENERATED_BODY()

public:
    // 攻撃威力通知を管理対象へ追加し、関連状態を更新する。
    UFUNCTION(BlueprintCallable)
    void AddAtkPowerNotification(const FText& Message);

	// 攻撃威力を解除し、再利用可能な初期状態へ戻す。
	UFUNCTION(BlueprintCallable)
	void ClearAtkPowerNotifications();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> NotificationBox;

	UPROPERTY(EditDefaultsOnly, Category = "Notification")
	TSubclassOf<UPlayerAtkPowerNotificationEntry> NotificationEntryClass;
};
