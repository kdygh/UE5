// アイテムの取得効果を処理する。

#include "Item/IncreaseAtkPower.h"
#include "Components/SphereComponent.h"
#include "../../MyPlayer.h"
#include "../Public/AttributeSet/PlayerAttributeSet.h"
#include "NiagaraComponent.h"
#include "Subsystem/GameAudioSubsystem.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
AIncreaseAtkPower::AIncreaseAtkPower()
{

	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("root"));
	SetRootComponent(Root);

	SphereItem = CreateDefaultSubobject<USphereComponent>(TEXT("RecoverHP"));
	SphereItem->SetupAttachment(Root);

	ItemShape = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ItemShape"));
	ItemShape->SetupAttachment(Root);

	ItemType = EItemType::INCREASE_ATKPOWER;
}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void AIncreaseAtkPower::BeginPlay()
{
	Super::BeginPlay();

    InitializeItem("IncreaseAtkPower");
}


// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void AIncreaseAtkPower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// 構成要素の生成完了後に、相互参照とイベントを接続する。
void AIncreaseAtkPower::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    SphereItem->OnComponentBeginOverlap.AddDynamic(this, &AIncreaseAtkPower::OnOverlapsBegin);
}

// 重なりを検知した相手の種類を確認し、該当するダメージまたは取得効果を適用する。
void AIncreaseAtkPower::OnOverlapsBegin_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Something hits"));
    ECollisionChannel OtherChannel = OtherComp->GetCollisionObjectType();

    switch (OtherChannel)
    {
    case ECC_GameTraceChannel4:
    {
        if (!OtherActor || OtherActor == this)
        {
            return;
        }

        if (AMyPlayer* Player = Cast<AMyPlayer>(OtherActor))
        {
            UE_LOG(LogTemp, Warning, TEXT("Player overlapped: %s"), *Player->GetName());


            ApplyEffect(Player, ItemType);
			Player->IncreasePlayerAtkPower(Amount);
            Player->ShowItemDesc(FText::Format(
                FText::FromString(ItemDesc),
                FText::AsNumber(Amount)
            ));

            if (UGameAudioSubsystem* AudioSubsystem =
                GetGameInstance()->GetSubsystem<UGameAudioSubsystem>())
            {
                AudioSubsystem->PlayItemGetSound(GetActorLocation());
            }

            Destroy();
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Overlapped actor is not a player."));
        }
    }
    break;
    }
}
