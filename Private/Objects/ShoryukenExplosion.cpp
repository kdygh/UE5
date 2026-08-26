// ゲーム内オブジェクトの動作を処理する。

#include "Objects/ShoryukenExplosion.h"
#include "NiagaraComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include "AttributeSet/PlayerAttributeSet.h"
#include "../MyPlayer.h"
#include "Subsystem/EffectSubsystem.h"
#include "Subsystem/GameAudioSubsystem.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
AShoryukenExplosion::AShoryukenExplosion()
{

	PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("root"));
    SetRootComponent(Root);

    // 現在は使用していない処理。
    //NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
    //NiagaraComponent->SetupAttachment(Root);
}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void AShoryukenExplosion::BeginPlay()
{
	Super::BeginPlay();

}

// プールから取得したアクターの位置と状態を再設定し、再び有効にする。
void AShoryukenExplosion::ActivateFromPool(const FTransform& SpawnTransform)
{
	bInUse = true;

	SetActorTransform(SpawnTransform);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);

	GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle,
		FTimerDelegate::CreateUObject(this, &AShoryukenExplosion::GenerateExplosion),
		0.3f, true);
}


// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void AShoryukenExplosion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (currentTime >= Duration)
    {
        currentTime = 0.0f;
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("Explosion Deactivated"));
        if (GetWorld())
        {
            GetWorld()->GetTimerManager().ClearTimer(ExplosionTimerHandle);
        }
        DeactivateToPool();
        return;
    }

    currentTime += DeltaTime;

    FVector NewLocation = GetActorLocation() + (GetActorForwardVector() * MoveSpeed * DeltaTime);
    SetActorLocation(NewLocation, true);
}

// 爆発を生成し、位置・所有者・初期状態を設定する。
void AShoryukenExplosion::GenerateExplosion()
{
	UE_LOG(LogTemp, Warning, TEXT("Generating Explosion at location: %s"), *GetActorLocation().ToString());
    TArray<AActor*> OverlappedActors;
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    TArray<AActor*> ActorsToIgnore;

    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel4));
    ActorsToIgnore.Add(this);

    AMyPlayer* Player = Cast<AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
    if (!Player)
    {
        return;
    }

    FVector Dir = Player->GetActorLocation() - GetActorLocation();
    Dir.Z = 0.0f;

    if (!Dir.IsNearlyZero())
    {
        FRotator TargetRot = Dir.Rotation();
        SetActorRotation(TargetRot);
    }

    const FVector Center = GetActorLocation();

    bool bHit = UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        Center,
        Radius,
        ObjectTypes,
        AMyPlayer::StaticClass(),
        ActorsToIgnore,
        OverlappedActors
    );

    if (UEffectSubsystem* EffectSubsystem = GetWorld()->GetSubsystem<UEffectSubsystem>())
    {
        EffectSubsystem->SpawnExplosionFX(GetActorLocation(), FRotator::ZeroRotator);
    }

    UGameAudioSubsystem* AudioSubsystem = GetGameInstance()->GetSubsystem<UGameAudioSubsystem>();
    if (AudioSubsystem)
    {
        AudioSubsystem->PlayExplosionSound(GetActorLocation());
    }

    for (AActor* Actor : OverlappedActors)
    {
        if (Player == Cast<AMyPlayer>(Actor))
        {
            if (Player->GetIsParrying())
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
