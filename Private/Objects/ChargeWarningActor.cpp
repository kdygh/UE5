// ゲーム内オブジェクトの動作を処理する。

#include "Objects/ChargeWarningActor.h"
#include "Components/DecalComponent.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
AChargeWarningActor::AChargeWarningActor()
{
    PrimaryActorTick.bCanEverTick = true;

    DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
    SetRootComponent(DecalComp);
}

// 対象の参照と初期状態を設定し、使用可能な状態にする。
void AChargeWarningActor::InitializeWarning(AActor* InSource, AActor* InTarget, float InDuration)
{
    SourceActor = InSource;
    TargetActor = InTarget;
    RemainingTime = InDuration;
}

// 対象に関するイベントを受け取り、関連状態を更新する。
void AChargeWarningActor::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (!DecalComp || !BaseMaterial)
    {
        return;
    }

    if (!MID)
    {
        MID = UMaterialInstanceDynamic::Create(BaseMaterial, this);
        DecalComp->SetDecalMaterial(MID);
    }


    DecalComp->DecalSize = FVector(128.f, DecalWidth, DecalLength);

    MID->SetScalarParameterValue(TEXT("DecalLength"), DecalLength);
    MID->SetScalarParameterValue(TEXT("DecalWidth"), DecalWidth);
    MID->SetScalarParameterValue(TEXT("ArrowTileLength"), ArrowTileLength);
    MID->SetScalarParameterValue(TEXT("ArrowTileWidth"), ArrowTileWidth);
    MID->SetScalarParameterValue(TEXT("ScrollSpeed"), ScrollSpeed);
}


// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void AChargeWarningActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!IsValid(SourceActor) || !IsValid(TargetActor))
    {
        Destroy();
        return;
    }

    RemainingTime -= DeltaTime;

    const FVector Start = SourceActor->GetActorLocation();
    const FVector End = TargetActor->GetActorLocation();

    FVector Direction = End - Start;
    Direction.Z = 0.f;

    const float Distance = Direction.Size();
    if (Distance <= KINDA_SMALL_NUMBER)
    {
        return;
    }

    const FVector MidPoint = (Start + End) * 0.5f;

    FRotator Rotation = Direction.Rotation();
    Rotation.Pitch = -90.f;
	Rotation.Yaw += 90.f;

    const float GroundOffset = -90.f;

    SetActorLocation(
        MidPoint + FVector(0.f, 0.f, GroundOffset)
    );

    SetActorRotation(Rotation);

    DecalComp->DecalSize = FVector(
        10.f,
        Distance * 0.5f,
        WarningWidth * 0.5f
    );

    if (RemainingTime <= 0.f)
    {
        Destroy();
    }
}
