// 画面表示と操作を処理する。

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemNotificationPanel.generated.h"


class UItemNotificationEntry;
class UVerticalBox;

// 画面表示と操作を処理する役割を持つ。
UCLASS()
class PJ26_API UItemNotificationPanel : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void AddItemNotification(
        const FText& Message,
        float DisplayDuration = 3.f
    );

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UVerticalBox> NotificationBox;

    UPROPERTY(EditDefaultsOnly, Category = "Notification")
    TSubclassOf<UItemNotificationEntry> NotificationEntryClass;

    UPROPERTY(EditAnywhere, Category = "Notification")
    int32 MaxNotificationCount = 5;
};
