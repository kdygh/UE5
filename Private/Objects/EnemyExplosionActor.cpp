// ゲーム内オブジェクトの動作を処理する。

#include "Objects/EnemyExplosionActor.h"
#include "NiagaraComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include "EnemyOrigin.h"
#include "Subsystem/EffectSubsystem.h"
#include "AttributeSet/EnemyAttributeSet.h"
#include "AttributeSet/PlayerAttributeSet.h"
#include "../../MyPlayer.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
AEnemyExplosionActor::AEnemyExplosionActor()
{

	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("root"));
	SetRootComponent(Root);

	// 現在は使用していない処理。
	//NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	//NiagaraComponent->SetupAttachment(Root);
}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void AEnemyExplosionActor::BeginPlay()
{
	Super::BeginPlay();
}


// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void AEnemyExplosionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (currentTime >= Duration)
    {
        bIsActive = false;
        currentTime = 0.0f;
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("Explosion Deactivated"));
        DeactivateToPool();
        return;
    }

    currentTime += DeltaTime;

    if (bIsActive == false)
    {
        bIsActive = true;
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Explosion Activated"));
        PerformOverlapAction();
    }
}

// 範囲内の対象を取得し、重複を避けながらダメージまたは効果を適用する。
void AEnemyExplosionActor::PerformOverlapAction()
{
    GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Performing Shockwave Overlap"));
    TArray<AActor*> OverlappedActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    TArray<AActor*> ActorsToIgnore;

    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel4));
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel5));
    ActorsToIgnore.Add(this);

    const FVector Center = GetActorLocation();

    bool bHit = UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        Center,
        Radius,
        ObjectTypes,
        nullptr,
        ActorsToIgnore,
        OverlappedActors
    );

    if (!bHit)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Explosion Overlap Failed"));
        DrawDebugSphere(GetWorld(), Center, Radius, 24, FColor::Red, false, 1.0f);
        return;
    }

    for (AActor* Actor : OverlappedActors)
    {
        if (AEnemyOrigin* Enemy = Cast<AEnemyOrigin>(Actor))
        {
            UE_LOG(LogTemp, Warning, TEXT("Explosion Hit: %s"), *Enemy->GetName());

            Enemy->CalCollisionDamage(ExplosionDamage);
        }

        if(AMyPlayer* Player = Cast<AMyPlayer>(Actor))
        {
            if(Player->GetIsParrying())
            {
                continue;
			}

            UE_LOG(LogTemp, Warning, TEXT("Explosion Hit: %s"), *Player->GetName());
            UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent();
            const UPlayerAttributeSet* AttrSet = ASC->GetSet<UPlayerAttributeSet>();
            Player->CalCollisionDamage(ExplosionDamage, nullptr);
		}
    }
}
