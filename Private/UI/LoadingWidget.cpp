// 画面表示と操作を処理する。

#include "UI/LoadingWidget.h"

// 画面部品の初回生成時に、必要な参照と初期表示状態を設定する。
void ULoadingWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (FadeOutAnimation)
    {
        FWidgetAnimationDynamicEvent FinishedDelegate;

        FinishedDelegate.BindDynamic(
            this,
            &ULoadingWidget::OnFadeOutFinished
        );

        BindToAnimationFinished(
            FadeOutAnimation,
            FinishedDelegate
        );
    }

    if (LoadingAnimation)
    {
        PlayAnimation(
            LoadingAnimation,
            0.f,
            0,
            EUMGSequencePlayMode::Forward,
            1.f
        );
    }
}

// フェードを設定された条件で再生し、再生状態を更新する。
void ULoadingWidget::PlayFadeOut()
{
    if (!LoadingAnimation)
    {
        RemoveFromParent();
        return;
    }

    if (!FadeOutAnimation)
    {
        RemoveFromParent();
        return;
    }

    PlayAnimation(FadeOutAnimation);
}

// 処理の完了を確認し、後続の状態または画面へ移行する。
void ULoadingWidget::OnFadeOutFinished()
{
    RemoveFromParent();
}
