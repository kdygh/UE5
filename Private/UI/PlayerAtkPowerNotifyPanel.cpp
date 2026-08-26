// 画面表示と操作を処理する。

#include "UI/PlayerAtkPowerNotifyPanel.h"
#include "UI/PlayerAtkPowerNotificationEntry.h"
#include "Components/VerticalBox.h"

// 攻撃威力通知を管理対象へ追加し、関連状態を更新する。
void UPlayerAtkPowerNotifyPanel::AddAtkPowerNotification(const FText& Message)
{
    if (!NotificationBox || !NotificationEntryClass)
    {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("NotificationBox or NotificationEntryClass is null"));
        return;
    }

    UPlayerAtkPowerNotificationEntry* NewEntry =
        CreateWidget<UPlayerAtkPowerNotificationEntry>(
            GetOwningPlayer(),
            NotificationEntryClass
        );

    if (!NewEntry)
    {
        return;
    }


    NotificationBox->AddChildToVerticalBox(NewEntry);

    NewEntry->InitializeNotification(
        Message
    );
}

// 攻撃威力を解除し、再利用可能な初期状態へ戻す。
void UPlayerAtkPowerNotifyPanel::ClearAtkPowerNotifications()
{
    UWidget* OldestEntry = NotificationBox->GetChildAt(0);

    if (OldestEntry)
    {
        OldestEntry->RemoveFromParent();
    }
}
