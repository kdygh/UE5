// 敵固有の行動と戦闘を処理する。

#include "UI/Enemy/EnemyHPWidget.h"
#include "Components/ProgressBar.h"
#include "Components/WrapBox.h"
#include "../../../Public/UI/Enemy/DebuffIconWidget.h"
#include "../../../Public/EnemyOrigin.h"
#include "../../../Public/UI/Enemy/ActiveDebuffInfo.h"
#include "../../../Public/UI/StatusBuildUpIcon.h"
#include <Components/TextBlock.h>

// 画面部品の生成後に、子要素・イベント・初期表示を設定する。
void UEnemyHPWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateHp();
	UpdateSp();
    RefreshDebuffs();

    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            DebuffRefreshTimerHandle,
            this,
            &UEnemyHPWidget::RefreshDebuffs,
            0.1f,
            true
        );
    }
}

// 画面部品の破棄前に、外部イベントと保持参照を解除する。
void UEnemyHPWidget::NativeDestruct()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("EnemyHPWidget NativeDestruct called"));

    if (UWorld* World = GetWorld())
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("EnemyHPWidget Destructed - Timer Cleared"));
        World->GetTimerManager().ClearTimer(DebuffRefreshTimerHandle);
    }

    DebuffRefreshTimerHandle.Invalidate();

    Super::NativeDestruct();
}

// 所有者敵を指定された値へ更新する。
void UEnemyHPWidget::SetOwnerEnemy(AEnemyOrigin* InEnemy)
{
	OwnerEnemy = InEnemy;
	UpdateHp();
	UpdateSp();
	RefreshDebuffs();
}

// 体力を現在の実行状態に合わせて再計算し、関連要素へ反映する。
void UEnemyHPWidget::UpdateHp()
{
	if (!HPBar || !OwnerEnemy)
	{
		return;
	}

	HPBar->SetPercent(OwnerEnemy->GetHpPercent());
}

// スタミナを現在の実行状態に合わせて再計算し、関連要素へ反映する。
void UEnemyHPWidget::UpdateSp()
{
	if(!SPBar || !OwnerEnemy)
	{
		return;
	}

	SPBar->SetPercent(OwnerEnemy->GetSpPercent());
}

// 有効な状態異常情報を取得し、アイコンの追加・更新・削除を画面へ反映する。
void UEnemyHPWidget::RefreshDebuffs()
{
    if (!IsValid(OwnerEnemy))
    {
        return;
    }

    if (!DebuffWrapBox || !OwnerEnemy || !DebuffIconWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid refs - WrapBox:%s OwnerEnemy:%s IconClass:%s"),
            *GetNameSafe(DebuffWrapBox),
            *GetNameSafe(OwnerEnemy),
            *GetNameSafe(DebuffIconWidgetClass));
        return;
    }

    DebuffWrapBox->ClearChildren();

    const TArray<FActiveDebuffInfo>& Debuffs = OwnerEnemy->GetActiveDebuffs();

    for (const FActiveDebuffInfo& Debuff : Debuffs)
    {
        UDebuffIconWidget* IconWidget = CreateWidget<UDebuffIconWidget>(this, DebuffIconWidgetClass);

        if (!IconWidget)
        {
            continue;
        }

        DebuffWrapBox->AddChildToWrapBox(IconWidget);

        IconWidget->SetIcon(Debuff.IconTexture);

        const float Progress = Debuff.TotalDuration > 0.f
            ? Debuff.RemainingTime / Debuff.TotalDuration
            : 0.f;

        IconWidget->SetProgress(Progress);
    }
}
