// ゲーム全体で保持する状態を管理する。

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"


// ゲーム全体で保持する状態を管理する役割を持つ。
UCLASS()
class PJ26_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	void HandleWorldInit(UWorld* World, const UWorld::InitializationValues IVS);

protected:
	// ゲーム全体で使用する参照と初期状態を準備する。
	virtual void Init() override;
	// ゲーム終了時に登録済み処理と保持している参照を解放する。
	virtual void Shutdown() override;

public:
	// 現在は使用していない処理。
	//UPROPERTY()
	//class ARoomsManager* RoomsManager;
};
