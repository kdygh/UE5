// ゲーム全体で共有する機能を管理する。

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EffectActorSubsystem.generated.h"

class APooledActor;

// ゲーム全体で共有する機能を管理する役割を持つ。
USTRUCT()
struct FActorPool
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TObjectPtr<APooledActor>> Actors;
};


// ゲーム全体で共有する機能を管理する役割を持つ。
UCLASS()
class PJ26_API UEffectActorSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// 指定クラスのアクターを必要数だけ事前生成し、非表示状態でプールへ登録する。
	UFUNCTION(BlueprintCallable, Category = "Pool")
	void PrewarmPool(TSubclassOf<APooledActor> ActorClass, int32 Count);

	// プール内のアクターを破棄し、クラスごとの管理情報をすべて削除する。
	UFUNCTION(BlueprintCallable, Category = "Pool")
	void ClearPool();

	// 指定クラスの利用可能なアクターを取得し、位置と移動方向を設定して有効にする。
	UFUNCTION(BlueprintCallable, Category = "Pool")
	APooledActor* AcquireActor(TSubclassOf<APooledActor> ActorClass, const FTransform& SpawnTransform, const FVector& MoveDirection);

	// 使用済みアクターを無効化し、再利用できるよう対応するプールへ戻す。
	UFUNCTION(BlueprintCallable, Category = "Pool")
	void ReleaseActor(APooledActor* Actor);

private:
	UPROPERTY()
	TMap<TSubclassOf<APooledActor>, FActorPool> Pools;
};
