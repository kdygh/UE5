// 画面表示と操作を処理する。

#include "UI/PlayerAtkPowerNotificationEntry.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"

// 表示する文言と継続時間を設定し、通知の表示準備を行う。
void UPlayerAtkPowerNotificationEntry::InitializeNotification(const FText& Message)
{
    if (AtkPowerText)
    {
        AtkPowerText->SetText(Message);
    }
}

// 画面部品の生成後に、子要素・イベント・初期表示を設定する。
void UPlayerAtkPowerNotificationEntry::NativeConstruct()
{
    Super::NativeConstruct();
}

// 画面部品の破棄前に、外部イベントと保持参照を解除する。
void UPlayerAtkPowerNotificationEntry::NativeDestruct()
{
    Super::NativeDestruct();
}

// 表示時間の終了後にフェードアウトアニメーションを開始する。
void UPlayerAtkPowerNotificationEntry::BeginFadeOut()
{
    if (FadeOut)
    {
        FWidgetAnimationDynamicEvent FinishedEvent;
        FinishedEvent.BindDynamic(
            this,
            &UPlayerAtkPowerNotificationEntry::HandleFadeOutFinished
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
void UPlayerAtkPowerNotificationEntry::HandleFadeOutFinished()
{
    RemoveFromParent();
}
