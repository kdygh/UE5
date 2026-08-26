// 画面表示と操作を処理する。

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingWidget.generated.h"


// 画面表示と操作を処理する役割を持つ。
UCLASS()
class PJ26_API ULoadingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    // フェードを設定された条件で再生し、再生状態を更新する。
    void PlayFadeOut();

protected:
    // 画面部品の初回生成時に、必要な参照と初期表示状態を設定する。
    virtual void NativeOnInitialized() override;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    TObjectPtr<UWidgetAnimation> FadeOutAnimation;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    TObjectPtr<UWidgetAnimation> LoadingAnimation;

private:
    // 処理の完了を確認し、後続の状態または画面へ移行する。
    UFUNCTION()
    void OnFadeOutFinished();
};
