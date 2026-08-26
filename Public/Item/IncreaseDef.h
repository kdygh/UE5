// アイテムの取得効果を処理する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.h"
#include "IncreaseDef.generated.h"

class AMyPlayer;

// アイテムの取得効果を処理する役割を持つ。
UCLASS()
class PJ26_API AIncreaseDef : public AItem
{
	GENERATED_BODY()

public:

	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	AIncreaseDef();

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;

public:

	// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
	virtual void Tick(float DeltaTime) override;
	// 構成要素の生成完了後に、相互参照とイベントを接続する。
	virtual void PostInitializeComponents() override;

public:
	// 重なりを検知した相手の種類を確認し、該当するダメージまたは取得効果を適用する。
	UFUNCTION(BlueprintNativeEvent, Category = "Hit")
	void OnOverlapsBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
