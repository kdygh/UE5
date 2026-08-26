// 画面表示と操作を処理する。

#include "UI/ItemNotificationEntry.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "TimerManager.h"

// 表示する文言と継続時間を設定し、通知の表示準備を行う。
void UItemNotificationEntry::InitializeNotification(const FText& Message, float DisplayDuration)
{
    if (EffectText)
    {
        EffectText->SetText(Message);
    }

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            DisplayTimerHandle,
            this,
            &UItemNotificationEntry::BeginFadeOut,
            DisplayDuration,
            false
        );
    }
}

// 画面部品の生成後に、子要素・イベント・初期表示を設定する。
void UItemNotificationEntry::NativeConstruct()
{
    Super::NativeConstruct();
}

// 画面部品の破棄前に、外部イベントと保持参照を解除する。
void UItemNotificationEntry::NativeDestruct()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(DisplayTimerHandle);
    }

    Super::NativeDestruct();
}

// 表示時間の終了後にフェードアウトアニメーションを開始する。
void UItemNotificationEntry::BeginFadeOut()
{
    if (FadeOut)
    {
        FWidgetAnimationDynamicEvent FinishedEvent;
        FinishedEvent.BindDynamic(
            this,
            &UItemNotificationEntry::HandleFadeOutFinished
        );

        BindToAnimationFinished(FadeOut, FinishedEvent);
        PlayAnimation(FadeOut);
    }
    else
    {
        RemoveFromParent();
    }
}

// フェードアウト完了後に通知を画面から削除する。
void UItemNotificationEntry::HandleFadeOutFinished()
{
    RemoveFromParent();
}
