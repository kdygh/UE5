// ゲーム全体で保持する状態を管理する。

#include "MyGameInstance.h"
#include "RoomsManager.h"

// ゲーム全体で使用する参照と初期状態を準備する。
void UMyGameInstance::Init()
{
    Super::Init();

    // 現在は使用していない処理。
    //FWorldDelegates::OnPostWorldInitialization.AddUObject(
    //    this,
    //    &UMyGameInstance::HandleWorldInit
    //);
}

// ゲーム終了時に登録済み処理と保持している参照を解放する。
void UMyGameInstance::Shutdown()
{
    // 現在は使用していない処理。
    //FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);
	Super::Shutdown();
}

// 現在は処理を実装しておらず、呼び出し口だけを定義する。
void UMyGameInstance::HandleWorldInit(UWorld* World, const UWorld::InitializationValues IVS)
{
    // 現在は使用していない処理。
    //if (World->IsGameWorld())
    //{
    //    RoomsManager = World->SpawnActor<ARoomsManager>();
    //}
}
