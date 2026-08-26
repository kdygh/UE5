// 敵の人工知能による行動を処理する。

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "MyBTTask_BlackboardBase.generated.h"


// 敵の人工知能による行動を処理する役割を持つ。
UCLASS()
class PJ26_API UMyBTTask_BlackboardBase : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	UMyBTTask_BlackboardBase();

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	// 対象を取得し、呼び出し元へ返す。
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowPrivateAccess = true))
	float SearchRadius = 200.0f;
};
