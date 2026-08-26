// ゲーム開始と進行を管理する。

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MyPj26GameMode.generated.h"


class ARoom;
class ULoadingWidget;

// ゲーム開始と進行を管理する役割を持つ。
UCLASS()
class PJ26_API AMyPj26GameMode : public AGameMode
{
	GENERATED_BODY()

protected:

	// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
	virtual void BeginPlay() override;
	// 構成要素の生成完了後に、相互参照とイベントを接続する。
	virtual void PostInitializeComponents() override;

public:
	// 読み込むステージとロード画面を設定した後、レベル遷移を開始する。
	UFUNCTION()
	void LoadStage();
	// 対象レベルの表示完了後、ロード画面を閉じてゲーム進行を再開する。
	UFUNCTION()
	void OnLevel1Shown();
	// 新しいゲームに必要な状態を初期化し、最初のステージへ移動する。
	UFUNCTION()
	void StartNewGame();

	// ロード画面部品を生成して表示し、入力状態を調整する。
	void ShowLoadingWidget();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room Template", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<ARoom>> CombatRoomTemplate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room Template", meta = (AllowPrivateAccess = "true"))

	TArray<TSubclassOf<ARoom>> ItemRoomTemplate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room Template", meta = (AllowPrivateAccess = "true"))

	TArray<TSubclassOf<ARoom>> TrapRoomTemplate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room Template", meta = (AllowPrivateAccess = "true"))

	TArray<TSubclassOf<ARoom>> BossRoomTemplate;

	UPROPERTY()
	TObjectPtr<ULoadingWidget> LoadingWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loading", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ULoadingWidget> LoadingWidgetClass;
};
