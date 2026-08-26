// ゲーム全体で共有する機能を管理する。

#include "Subsystem/EffectActorSubsystem.h"
#include "Objects/PooledActor.h"

// 指定クラスのアクターを必要数だけ事前生成し、非表示状態でプールへ登録する。
void UEffectActorSubsystem::PrewarmPool(TSubclassOf<APooledActor> ActorClass, int32 Count)
{
	if (!GetWorld() || !ActorClass)
	{
		return;
	}

	FActorPool& Pool = Pools.FindOrAdd(ActorClass);

	for (int32 i = 0; i < Count; ++i)
	{
		APooledActor* NewActor = GetWorld()->SpawnActor<APooledActor>(ActorClass, FTransform::Identity);
		if (NewActor)
		{
			NewActor->DeactivateToPool();
			Pool.Actors.Add(NewActor);
		}
	}
}

// プール内のアクターを破棄し、クラスごとの管理情報をすべて削除する。
void UEffectActorSubsystem::ClearPool()
{
    for (auto& PoolPair : Pools)
    {
        FActorPool& Pool = PoolPair.Value;

        for (APooledActor* Actor : Pool.Actors)
        {
            if (IsValid(Actor))
            {
                Actor->Destroy();
            }
        }

        Pool.Actors.Empty();
    }

    Pools.Empty();
}

// 指定クラスの利用可能なアクターを取得し、位置と移動方向を設定して有効にする。
APooledActor* UEffectActorSubsystem::AcquireActor(TSubclassOf<APooledActor> ActorClass, const FTransform& SpawnTransform, const FVector& MoveDirection)
{
    UWorld* World = GetWorld();

    if (!IsValid(World) || !ActorClass)
    {
        return nullptr;
    }

    FActorPool& Pool = Pools.FindOrAdd(ActorClass);


    Pool.Actors.RemoveAll(
        [World](APooledActor* Actor)
        {
            return !IsValid(Actor) ||
                Actor->GetWorld() != World ||
                Actor->IsActorBeingDestroyed();
        }
    );

    for (APooledActor* Actor : Pool.Actors)
    {
        if (!Actor->IsInUse())
        {
            Actor->SetMoveDirection(MoveDirection);
            Actor->ActivateFromPool(SpawnTransform);

            return Actor;
        }
    }


    APooledActor* NewActor =
        World->SpawnActor<APooledActor>(
            ActorClass,
            SpawnTransform
        );

    if (IsValid(NewActor))
    {
        NewActor->SetMoveDirection(MoveDirection);
        NewActor->ActivateFromPool(SpawnTransform);
        Pool.Actors.Add(NewActor);
    }

    return NewActor;
}

// 使用済みアクターを無効化し、再利用できるよう対応するプールへ戻す。
void UEffectActorSubsystem::ReleaseActor(APooledActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	Actor->DeactivateToPool();
}
