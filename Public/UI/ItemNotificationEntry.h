// 画面表示と操作を処理する。

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemNotificationEntry.generated.h"


class UTextBlock;
class UTexture2D;
class UWidgetAnimation;

// 画面表示と操作を処理する役割を持つ。
UCLASS()
class PJ26_API UItemNotificationEntry : public UUserWidget
{
	GENERATED_BODY()

public:
    void InitializeNotification(
        const FText& Message,
        float DisplayDuration
    );

protected:
    // 画面部品の生成後に、子要素・イベント・初期表示を設定する。
    virtual void NativeConstruct() override;
    // 画面部品の破棄前に、外部イベントと保持参照を解除する。
    virtual void NativeDestruct() override;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> EffectText;


    UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
    TObjectPtr<UWidgetAnimation> FadeOut;

private:
    FTimerHandle DisplayTimerHandle;

    // 表示時間の終了後にフェードアウトアニメーションを開始する。
    UFUNCTION()
    void BeginFadeOut();

    // フェードアウト完了後に通知を画面から削除する。
    UFUNCTION()
    void HandleFadeOutFinished();
};
