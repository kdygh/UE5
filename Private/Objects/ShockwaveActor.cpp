// ゲーム内オブジェクトの動作を処理する。

#include "Objects/ShockwaveActor.h"
#include "NiagaraComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include "EnemyOrigin.h"
#include "Subsystem/EffectSubsystem.h"
#include "AttributeSet/EnemyAttributeSet.h"
#include "AttributeSet/PlayerAttributeSet.h"
#include "../../MyPlayer.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
AShockwaveActor::AShockwaveActor()
{

	PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("root"));
    SetRootComponent(Root);

    NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
    NiagaraComponent->SetupAttachment(Root);
}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void AShockwaveActor::BeginPlay()
{
	Super::BeginPlay();
}


// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void AShockwaveActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (currentTime >= Duration)
    {
		bIsActive = false;
        currentTime = 0.0f;
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("Shockwave Deactivated"));
        DeactivateToPool();
        return;
    }

    currentTime += DeltaTime;

    if(bIsActive == false)
    {
        bIsActive = true;
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Shockwave Activated"));
        PerformOverlapActtion();
	}
}

// 範囲内の対象を取得し、重複を避けながら衝撃波の効果を適用する。
void AShockwaveActor::PerformOverlapActtion()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Performing Shockwave Overlap"));
    TArray<AActor*> OverlappedActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    TArray<AActor*> ActorsToIgnore;

    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel5));
    ActorsToIgnore.Add(this);

    const FVector Center = GetActorLocation();

    bool bHit = UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        Center,
        Radius,
        ObjectTypes,
        AEnemyOrigin::StaticClass(),
        ActorsToIgnore,
        OverlappedActors
    );

    if (!bHit)
    {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Shockwave Overlap Failed"));
        return;
    }

    UEffectSubsystem* EffectSubsystem = GetWorld()->GetSubsystem<UEffectSubsystem>();
    if(EffectSubsystem)
    {
        EffectSubsystem->SpawnShockwaveFX(Center, FRotator::ZeroRotator);
		EffectSubsystem->SpawnDistortionShockFX(Center, FRotator::ZeroRotator);
	}

	AMyPlayer* Player = Cast<AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());

    if (Player == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("No Player"));
        return;
    }
    UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent();
    const UPlayerAttributeSet* AttrSet = ASC->GetSet<UPlayerAttributeSet>();

    for (AActor* Actor : OverlappedActors)
    {
        if (AEnemyOrigin* Enemy = Cast<AEnemyOrigin>(Actor))
        {
            UE_LOG(LogTemp, Warning, TEXT("Shockwave Hit: %s"), *Enemy->GetName());

			Enemy->CalCollisionDamageSP(AttrSet->GetSPAtkPower());
        }
    }
}
