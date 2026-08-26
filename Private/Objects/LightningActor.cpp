// ゲーム内オブジェクトの動作を処理する。

#include "Objects/LightningActor.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
ALightningActor::ALightningActor()
{

	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("root"));
	SetRootComponent(Root);

	CapsuleLightning = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleLightning"));
	CapsuleLightning->InitCapsuleSize(50.f, 100.f);
	CapsuleLightning->SetupAttachment(Root);
	CapsuleLightning->SetCollisionProfileName(CollisionProfileName);

	// 現在は使用していない処理。
	//NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	//NiagaraComponent->SetupAttachment(Root);
}

// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void ALightningActor::BeginPlay()
{
	Super::BeginPlay();
}

// 構成要素の生成完了後に、相互参照とイベントを接続する。
void ALightningActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void ALightningActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(currentTime >= Duration)
	{
		currentTime = 0.0f;
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("Lightning Deactivated"));
		DeactivateToPool();
		return;
	}

	currentTime += DeltaTime;

	FHitResult Hit;
	FVector NewLocation = GetActorLocation() + (MoveDirection.GetSafeNormal() * MoveSpeed * DeltaTime);
	SetActorLocation(NewLocation, true, &Hit);
}
