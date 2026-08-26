// 画面表示と操作を処理する。

#include "UI/ItemNotificationPanel.h"
#include "UI/ItemNotificationEntry.h"
#include "Components/VerticalBox.h"

// アイテム通知を管理対象へ追加し、関連状態を更新する。
void UItemNotificationPanel::AddItemNotification(const FText& Message, float DisplayDuration)
{
    if (!NotificationBox || !NotificationEntryClass)
    {
        return;
    }

    UItemNotificationEntry* NewEntry =
        CreateWidget<UItemNotificationEntry>(
            GetOwningPlayer(),
            NotificationEntryClass
        );

    if (!NewEntry)
    {
        return;
    }


    NotificationBox->AddChildToVerticalBox(NewEntry);

    NewEntry->InitializeNotification(
        Message,
        DisplayDuration
    );


    while (NotificationBox->GetChildrenCount() > MaxNotificationCount)
    {
        UWidget* OldestEntry = NotificationBox->GetChildAt(0);

        if (OldestEntry)
        {
            OldestEntry->RemoveFromParent();
        }
    }
}
