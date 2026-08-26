// プレイヤーの入力と戦闘を処理する。

#include "MyPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyGhostActor.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "MyAnimInstancePj26.h"
#include "Public/MySword.h"
#include <Kismet/GameplayStatics.h>
#include "TimerManager.h"
#include "Subsystem/EffectSubsystem.h"
#include "Subsystem/EffectActorSubsystem.h"
#include "Subsystem/CombatFeedbackSubsystem.h"
#include "Subsystem/GameFlowSubsystem.h"
#include "Subsystem/GameAudioSubsystem.h"
#include "Objects/ShockwaveActor.h"
#include "Public/UI/Player/PlayerHpWidget.h"
#include "Public/UI/LockonWidget.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet/PlayerAttributeSet.h"
#include "AttributeSet/EnemyAttributeSet.h"
#include "Public/Data/PlayerInfo.h"
#include "GameplayEffect.h"
#include <AbilitySystemBlueprintLibrary.h>
#include <NiagaraFunctionLibrary.h>
#include "Public/UI/ItemNotificationPanel.h"
#include "Public/UI/PlayerAtkPowerNotifyPanel.h"
#include "Public/EnemyOrigin.h"
#include "Public/Objects/DeathCameraActor.h"
#include "Public/Components/CameraLockOnComponent.h"
#include "Public/Components/WeaponTrailComponent.h"


// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
AMyPlayer::AMyPlayer()
{

	PrimaryActorTick.bCanEverTick = true;
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 300.f;
    SpringArm->bUsePawnControlRotation = true;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
    Camera->bUsePawnControlRotation = false;

    LockOnComponent = CreateDefaultSubobject<UCameraLockOnComponent>(TEXT("LockOnComponent"));
    LockOnComponent->SetCamera(Camera);
    LockOnComponent->SetOwnerPlayer(this);

    WeaponTrailComponent = CreateDefaultSubobject<UWeaponTrailComponent>(TEXT("WeaponTrailComponent"));


    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

    SwordComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Weapon"));
    SwordComponent->SetupAttachment(GetMesh(), TEXT("Weapon"));
	SwordComponent->SetChildActorClass(TSubclassOf<AActor>(StaticLoadClass(AActor::StaticClass(), nullptr, TEXT("/Game/characters/player/MySword.MySword_C"))));
	SwordComponent->SetRelativeLocation(FVector::ZeroVector);

	CapsuleParry = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Parry"));
	CapsuleParry->SetupAttachment(GetMesh(), TEXT("pelvis"));
    CapsuleParry->SetRelativeLocation(FVector::ZeroVector);


    GhostTrailEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("GhostTrail"));
    GhostTrailEffect->SetupAttachment(RootComponent);
	GhostTrailEffect->bAutoActivate = false;
	GhostTrailEffect->Deactivate();

    AbilitySystemComp = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("PlayerAbilitySystemComp"));
    AttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(TEXT("PlayerAttributeSet"));
}


// ゲーム開始時に必要な参照・イベント・初期状態を設定する。
void AMyPlayer::BeginPlay()
{
	Super::BeginPlay();

    if (SwordComponent && GetMesh())
    {
        SwordComponent->AttachToComponent(
            GetMesh(),
            FAttachmentTransformRules::SnapToTargetNotIncludingScale,
            TEXT("Weapon")
        );

        SwordComponent->GetChildActor()->SetOwner(this);
    }

	AnimInst = Cast<UMyAnimInstancePj26>(GetMesh()->GetAnimInstance());

    AnimInst->IsAttacking = false;
    AnimInst->IsAttackingStrong = false;

    CapsuleParry->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CapsuleParry->SetGenerateOverlapEvents(false);

    if (PlayerHpWidgetClass)
    {
        PlayerHpWidgetInstance = CreateWidget<UPlayerHpWidget>(GetWorld(), PlayerHpWidgetClass);
        if (PlayerHpWidgetInstance)
        {
            PlayerHpWidgetInstance->AddToViewport();
            PlayerHpWidgetInstance->SetOwnerPlayer(this);
        }
    }

    // 現在は使用していない処理。
    //if (PlayerLockonWidgetClass)
    //{
    //    PlayerLockonWidgetInstance = CreateWidget<ULockonWidget>(GetWorld(), PlayerLockonWidgetClass);
    //    if (PlayerLockonWidgetInstance)
    //    {
    //        PlayerLockonWidgetInstance->AddToViewport();
    //        PlayerLockonWidgetInstance->HideLockOnMarker();
    //    }
    //}

    if (PlayerInfoTable)
    {
        static const FString ContextString(TEXT("Player Info Context"));
        FName RowName = TEXT("Player");

        FPlayerInfo* PlayerInfo = PlayerInfoTable->FindRow<FPlayerInfo>(RowName, ContextString);

        if (PlayerInfo)
        {
            PlayerMaxHP = PlayerInfo->HP;
            PlayerMaxSP = PlayerInfo->SP;
			PlayerAtkPower = PlayerInfo->AtkPower;
			PlayerDef = PlayerInfo->Def;
			PlayerSpeed = PlayerInfo->Speed;

            AttributeSet->SetMaxHp(PlayerMaxHP);
            AttributeSet->SetHp(PlayerMaxHP);
            AttributeSet->SetMaxSp(PlayerMaxSP);
            AttributeSet->SetSp(PlayerMaxSP);
			AttributeSet->SetDef(PlayerDef);
            AttributeSet->SetAtkPower(PlayerAtkPower);
			AttributeSet->SetSPAtkPower(PlayerInfo->SPAtkPower);
			AttributeSet->SetMoveSpeed(PlayerSpeed);
			AttributeSet->SetThunderDmg(PlayerInfo->ThunderDmg);
			AttributeSet->SetWindDmg(PlayerInfo->WindDmg);
			AttributeSet->SetFireDmg(PlayerInfo->FireDmg);
            AttributeSet->SetSpRegenRate(PlayerSPGenRate);

        }
    }

    if(UPlayerHpWidget* HPWidget = Cast<UPlayerHpWidget>(PlayerHpWidgetInstance))
    {
        HPWidget->SetOwnerPlayer(this);
	}

    if (ItemNotificationPanelClass)
    {
        APlayerController* PC = Cast<APlayerController>(GetController());

        ItemNotificationPanel =
            CreateWidget<UItemNotificationPanel>(
                PC,
                ItemNotificationPanelClass
            );

        if (ItemNotificationPanel)
        {
            ItemNotificationPanel->AddToViewport(10);
        }
    }
}

// 構成要素の生成完了後に、相互参照とイベントを接続する。
void AMyPlayer::PostInitializeComponents()
{
    Super::PostInitializeComponents();
}

// 終了時に登録済みイベントやタイマーを解除し、保持参照を整理する。
void AMyPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}


// 毎フレーム、経過時間に応じて継続中の状態と表示を更新する。
void AMyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (IsAlive == false)
    {
        return;
    }

    AnimInst->IsInAir = GetCharacterMovement()->IsFalling();

    FVector Vel = GetVelocity();
    Vel.Z = 0.f;
    AnimInst->Speed = Vel.Size();

    if(AtkQueue.IsEmpty() == false)
    {
        FName* temp = AtkQueue.Peek();
        OnAtk(*temp);
	}

    // 現在は使用していない処理。
    //UpdateLockOn(DeltaTime);
	RegenerateStamina(DeltaTime);
}


// 移動・視点・戦闘に使用する入力と処理関数を関連付ける。
void AMyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AMyPlayer::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AMyPlayer::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &AMyPlayer::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &AMyPlayer::LookUp);
    PlayerInputComponent->BindAxis(TEXT("SwitchLockOnTarget"), this, &AMyPlayer::SwitchLockOnInput);

    PlayerInputComponent->BindAction("Atk_Normal", IE_Pressed, this, &AMyPlayer::Atk_Normal);
    PlayerInputComponent->BindAction("Atk_Strong", IE_Pressed, this, &AMyPlayer::Atk_Strong);
    PlayerInputComponent->BindAction("Evade", IE_Pressed, this, &AMyPlayer::Evade);
    PlayerInputComponent->BindAction("Parry", IE_Pressed, this, &AMyPlayer::Parry);
    PlayerInputComponent->BindAction("LockOn", IE_Pressed, this, &AMyPlayer::ToggleLockOn);
    PlayerInputComponent->BindAction("SetCurrentAttackAttribute", IE_Pressed, this, &AMyPlayer::SetCurrentAttackAttribute);
    PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AMyPlayer::Pause);
}

// カメラ方向を基準に、前後入力をワールド上の移動方向へ変換する。
void AMyPlayer::MoveForward(float Value)
{
    ForwardInput = Value;

    if (AtkCheck() == true)
    {
        return;
    }

    if(IsParrying == true)
    {
        return;
	}

    if (Controller && Value != 0.f)
    {
        const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
        const FVector Dir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
        AddMovementInput(Dir, Value);
    }
}

// カメラ方向を基準に、左右入力をワールド上の移動方向へ変換する。
void AMyPlayer::MoveRight(float Value)
{
	RightInput = Value;

    if (AtkCheck() == true)
    {
        return;
    }

    if (IsParrying == true)
    {
        return;
    }

    if (Controller && Value != 0.f)
    {
        const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
        const FVector Dir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
        AddMovementInput(Dir, Value);
    }
}

// 左右の視点入力を回転へ反映する。
void AMyPlayer::Turn(float Value)
{
    if (IsLockingOn == true)
    {
        return;
    }

    AddControllerYawInput(Value);
}

// 上下の視点入力を回転へ反映する。
void AMyPlayer::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}

// スティック方向と再入力可能状態を確認し、左右の対象切り替えを一度だけ実行する。
void AMyPlayer::SwitchLockOnInput(float Value)
{
    if (!LockOnComponent->GetLockedTarget())
    {
        isLockOnSwitchTriggered = false;
        return;
    }

    constexpr float Threshold = 0.6f;


    if (FMath::Abs(Value) < Threshold)
    {
        isLockOnSwitchTriggered = false;
        return;
    }


    if (isLockOnSwitchTriggered)
    {
        return;
    }

    isLockOnSwitchTriggered = true;

    if (Value < 0.f)
    {
        SwitchLockOnLeft();
    }
    else
    {
        SwitchLockOnRight();
    }
}

// 現在の戦闘状態を確認した後、弱攻撃を入力待ち行列へ登録する。
void AMyPlayer::Atk_Normal()
{
    if (AtkCheck() == true)
    {
        return;
    }

    if (ComboCnt > 0)
    {
        StopAtk();
        return;
    }

    canCombo = true;
    ++LightAtkCnt;

    AttributeSet->SetAtkPower(AttributeSet->GetAtkPower() + (AttributeSet->GetAtkPower() * 0.1f));


    if (LightAtkCnt >= (UINT)EATK::LIGHTATKMAX)
    {
        LightAtkCnt = (UINT)EATK::LIGHTATK1;
    }

    FString strAtk = FString::Printf(TEXT("Atk%d"), LightAtkCnt);
    AtkQueue.Enqueue((FName)strAtk);
}

// 現在の戦闘状態を確認した後、強攻撃を入力待ち行列へ登録する。
void AMyPlayer::Atk_Strong()
{
    if (AtkCheck() == true)
    {
        return;
    }

    if (canCombo == true)
    {
        DetermineNextAtk(LightAtkCnt, true);
		return;
    }

    canCombo = false;
    ++HeavyAtkCnt;

    if (HeavyAtkCnt >= (UINT)EATK::HEAVYATKMAX)
    {
        HeavyAtkCnt = (UINT)EATK::HEAVYATK1;
    }


    FString strHeavyAtk = FString::Printf(TEXT("HeavyAtk%d"), HeavyAtkCnt);

    AtkQueue.Enqueue((FName)strHeavyAtk);
}

// 現在の連続攻撃段階と入力を基に、次の攻撃を待ち行列へ登録する。
void AMyPlayer::Atk_Combo()
{
    if (AtkCheck() == true)
    {
        return;
    }

    UINT ComboCntTemp = (LightAtkCnt * 10) + ++ComboCnt;

    switch (LightAtkCnt)
    {
    case 1:
        if (ComboCntTemp > (UINT)EATK::COMB1MAX)
        {
            StopAtk();
        }
        break;
	case 2:
        if (ComboCntTemp > (UINT)EATK::COMB2MAX)
        {
            StopAtk();
        }
		break;
    case 3:
        if (ComboCntTemp > (UINT)EATK::COMB3MAX)
        {
            StopAtk();
        }
        break;
    case 4:
        if (ComboCntTemp > (UINT)EATK::COMB4MAX)
        {
            StopAtk();
        }
        break;
    }

    FString strComboAtk = FString::Printf(TEXT("COMBO%d"), ComboCntTemp);
    AtkQueue.Enqueue((FName)strComboAtk);
}

// 攻撃アニメーションの受付区間で、次の連続攻撃と先行入力を許可する。
void AMyPlayer::EnableNextCombo()
{
    AnimInst->IsAttacking = false;
    AnimInst->IsAttackingStrong = false;
}

// 現在の入力方向と行動状態を確認し、回避移動と無敵状態を開始する。
void AMyPlayer::Evade()
{
    if (IsEvading == true)
    {
        return;
    }
    if (IsAlive == false)
    {
        return;
    }
    StopAtk();
	IsEvading = true;
    AnimInst->SetIgnoreRootMotion(IsEvading);

    FVector InputDir =
        GetCurrentInputDirection();;

    if (InputDir.IsNearlyZero())
    {
        InputDir = -GetActorForwardVector();
    }

    InputDir.Normalize();
    SetActorRotation(InputDir.Rotation());

    LaunchCharacter(InputDir * DashPower, true, true);

    AnimInst->Montage_Play(EvadeMontage);

    if (UGameAudioSubsystem* AudioSubsystem =
        GetGameInstance()->GetSubsystem<UGameAudioSubsystem>())
    {
        AudioSubsystem->PlayDashSound(GetActorLocation());
    }
}

// 実行可能状態を確認した後、パリィアニメーションと衝突判定を開始する。
void AMyPlayer::Parry()
{
    if(IsParrying == true)
    {
        return;
	}

	StopAtk();

	IsParrying = true;

    AnimInst->Montage_Play(ParryMontage);
}

// 水平方向と指定された強さを使用し、キャラクターを押し戻して行動を制限する。
void AMyPlayer::Knockback(const FVector& Direction, const FRotator& Rotation, float Power)
{
    AnimInst->Montage_Play(AttackMontage);
    AnimInst->Montage_JumpToSection("Hit");
    SetActorRotation(Rotation);
    LaunchCharacter(Direction * Power, true, true);
}

// アニメーション通知名に応じて、攻撃判定・回避・パリィ・死亡状態を切り替える。
void AMyPlayer::OnAnimAction(FName ActionName)
{
    if (ActionName == "AtkStart")
    {
        AnimInst->IsAttacking = true;
    }
    if (ActionName == "AtkCollDisable")
    {
        if (AMySword* Sword = Cast<AMySword>(SwordComponent->GetChildActor()))
        {
			Sword->EndTraceAttack();
        }
    }
    else if (ActionName == "AtkCollEnable")
    {
        if (AMySword* Sword = Cast<AMySword>(SwordComponent->GetChildActor()))
        {
            Sword->StartTraceAttack();
        }
    }
    else if (ActionName == "NextComboEnable")
    {
        EnableNextCombo();
    }
    else if(ActionName == "NextHeavyAtkEnable")
    {
        EnableNextCombo();
	}
    else if(ActionName == "AtkEnd")
    {
        StopAtk();
	}
    else if(ActionName == "EvadeEnable")
    {
        EnableExtraEvading();
	}
    else if (ActionName == "EvadeEnd")
    {
		IsEvading = false;
        if (GhostTrailEffect)
        {
            GhostTrailEffect->Deactivate();
        }
        AnimInst->SetIgnoreRootMotion(IsEvading);
        GetCapsuleComponent()->SetCollisionResponseToChannel(
            ECC_GameTraceChannel5,
            ECR_Block);
    }
    else if(ActionName == "ParryEnable")
    {
        EnableExtraParrying();
    }
    else if (ActionName == "ParryEnd")
    {
		IsParrying = false;
	}
    else if (ActionName == "ActivateParryCollision")
    {
        CapsuleParry->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        CapsuleParry->SetGenerateOverlapEvents(true);
		GetCapsuleComponent()->SetGenerateOverlapEvents(false);
    }
    else if (ActionName == "DeactivateParryCollision")
    {
        CapsuleParry->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        CapsuleParry->SetGenerateOverlapEvents(false);
        GetCapsuleComponent()->SetGenerateOverlapEvents(true);
    }
    else if(ActionName == "EvadeStart")
    {
        GetCapsuleComponent()->SetCollisionResponseToChannel(
            ECC_GameTraceChannel5,
            ECR_Overlap);
	}
    else if (ActionName == "EvadeTrail"                                  )
    {
        SpawnAfterImage();
    }
    else if (ActionName == "EnableRagdoll")
    {

        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);


        USkeletalMeshComponent* MeshComp = GetMesh();
        if (MeshComp)
        {
            MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));


            MeshComp->SetSimulatePhysics(true);
            MeshComp->SetAllBodiesSimulatePhysics(true);
            MeshComp->SetEnableGravity(true);
            MeshComp->WakeAllRigidBodies();
            MeshComp->bBlendPhysics = true;
        }

        // 現在は使用していない処理。
        //GetWorldTimerManager().SetTimerForNextTick(this, &AMyPlayer::DisableAnimationAfterRagdoll);
    }

}

// 攻撃名に対応するアニメーション区間・攻撃属性・連続攻撃状態を設定する。
void AMyPlayer::OnAtk(FName AtkName)
{
    if (AtkCheck())
    {
		return;
    }

    if (IsEvading)
    {
		return;
    }

    if(CurrentAttackAttribute != EAttackAttribute::None)
    {
        ConsumeStamina(GetElementAttackStaminaCost(CurrentAttackAttribute));
	}

    AnimInst->IsAttacking = true;
    AnimInst->IsAttackingStrong = true;
    IsParrying = false;

    FVector InputDir =
        GetCurrentInputDirection();;

    if (InputDir.IsNearlyZero())
    {
        InputDir = GetActorForwardVector();
    }

    InputDir.Normalize();
    SetActorRotation(InputDir.Rotation());

    if (AtkName.ToString()[0] == 'A')
    {
		LaunchCharacter(InputDir * 1000, true, true);
        AnimInst->Montage_Play(AttackMontage);
        AnimInst->Montage_JumpToSection(AtkName);
    }
    else if (AtkName.ToString()[0] == 'H')
    {
        LaunchCharacter(InputDir * 1500, true, true);
        AnimInst->Montage_Play(HeavyAttackMontage);
        AnimInst->Montage_JumpToSection(AtkName);
    }
    else if (AtkName.ToString()[0] == 'C')
    {
        LaunchCharacter(InputDir * 1500, true, true);
        AnimInst->Montage_Play(ComboAttackMontage);
        AnimInst->Montage_JumpToSection(AtkName);
    }

    AnimInst->SetIgnoreRootMotion(true);
    AtkQueue.Dequeue(AtkName);

    if (UGameAudioSubsystem* AudioSubsystem =
        GetGameInstance()->GetSubsystem<UGameAudioSubsystem>())
    {
        AudioSubsystem->PlayWeaponSwingSound(GetActorLocation());
    }
}

// 体力割合を取得し、呼び出し元へ返す。
float AMyPlayer::GetHpPercent() const
{
    return PlayerMaxHP > 0.f ? AttributeSet->GetHp() / PlayerMaxHP : 0.f;
}

// スタミナ割合を取得し、呼び出し元へ返す。
float AMyPlayer::GetSpPercent() const
{
    return AttributeSet->GetMaxSp() > 0.f ? AttributeSet->GetSp() / AttributeSet->GetMaxSp() : 0.f;
}

// 体力を指定された値へ更新する。
void AMyPlayer::SetHP(float NewHP)
{
    if (PlayerHpWidgetInstance)
    {
        PlayerHpWidgetInstance->UpdateHp();
    }
}

// スタミナを指定された値へ更新する。
void AMyPlayer::SetSP(float NewSP)
{
    if (PlayerHpWidgetInstance)
    {
        PlayerHpWidgetInstance->UpdateSp();
    }
}

// 体力を上限の範囲内で指定量だけ回復する。
void AMyPlayer::RecoverHP(float Amount)
{
    float NewHP = 0.0f;
    NewHP += PlayerMaxHP * (Amount / 100.f);
    AttributeSet->SetHp(AttributeSet->GetHp() + NewHP);
    if (PlayerHpWidgetInstance)
    {
        PlayerHpWidgetInstance->UpdateHp();
    }

}

// プレイヤー最大体力を指定量だけ増加させ、関連表示へ反映する。
void AMyPlayer::IncreasePlayerMaxHP(float Amount)
{
    PlayerMaxHP += PlayerMaxHP * (Amount / 100.f);
	AttributeSet->SetMaxHp(PlayerMaxHP);
    if (PlayerHpWidgetInstance)
    {
        PlayerHpWidgetInstance->UpdateHp();
    }
}

// プレイヤー最大スタミナを指定量だけ増加させ、関連表示へ反映する。
void AMyPlayer::IncreasePlayerMaxSP(float Amount)
{
	PlayerMaxSP += PlayerMaxSP * (Amount / 100.f);
	AttributeSet->SetMaxSp(PlayerMaxSP);
    if (PlayerHpWidgetInstance)
    {
        PlayerHpWidgetInstance->UpdateSp();
    }
}

// プレイヤー攻撃威力を指定量だけ増加させ、関連表示へ反映する。
void AMyPlayer::IncreasePlayerAtkPower(float Amount)
{
	PlayerAtkPower += PlayerAtkPower * (Amount / 100.f);
	AttributeSet->SetAtkPower(PlayerAtkPower);
}

// プレイヤー防御力を指定量だけ増加させ、関連表示へ反映する。
void AMyPlayer::IncreasePlayerDef(float Amount)
{
	PlayerDef += PlayerDef * (Amount / 100.f);
	AttributeSet->SetDef(PlayerDef);
}

// プレイヤー速度を指定量だけ増加させ、関連表示へ反映する。
void AMyPlayer::IncreasePlayerSpeed(float Amount)
{
	PlayerSpeed += PlayerSpeed * (Amount / 100.f);
	AttributeSet->SetMoveSpeed(PlayerSpeed);
}

// 取得したアイテムの説明文を通知画面へ追加して表示する。
void AMyPlayer::ShowItemDesc(const FText message)
{
    ItemNotificationPanel->AddItemNotification(
        message,
        3.f
    );
}

// 現在攻撃属性を指定された値へ更新する。
void AMyPlayer::SetCurrentAttackAttribute()
{
    int32 num = (int32)CurrentAttackAttribute;

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC->IsInputKeyDown(EKeys::Q) ||
        PC->IsInputKeyDown(EKeys::Gamepad_LeftShoulder))
    {
        --num;

        if(num < (int32)EAttackAttribute::None)
        {
            num = (int32)EAttackAttribute::Wind;
		}
    }
    else if(PC->IsInputKeyDown(EKeys::E) ||
        PC->IsInputKeyDown(EKeys::Gamepad_RightShoulder))
    {
        ++num;

        if(num > (int32)EAttackAttribute::Wind)
        {
            num = (int32)EAttackAttribute::None;
		}
	}

    CurrentAttackAttribute = (EAttackAttribute)num;

    if (PlayerHpWidgetInstance)
    {
		PlayerHpWidgetInstance->UpdateAttackAttributeImage();
    }
}

// 攻撃属性に応じた武器軌跡を開始する。
void AMyPlayer::StartWeaponTrail()
{
    WeaponTrailComponent->StartWeaponTrail(GetMesh(), CurrentAttackAttribute);
}

// 現在の連続攻撃段階と強攻撃入力を基に、次の攻撃を決定する。
void AMyPlayer::DetermineNextAtk(UINT _comboCnt, bool _isHeavyAtk)
{
    if (canCombo == true)
    {
        AnimInst->IsAttacking = false;

        if (_isHeavyAtk == true)
        {
            Atk_Combo();
        }
    }
}

// 回避状態を有効にし、後続処理を受け付ける状態にする。
void AMyPlayer::EnableExtraEvading()
{
    if (IsAlive == false)
    {
        return;
    }

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC->IsInputKeyDown(EKeys::LeftShift) ||
        PC->IsInputKeyDown(EKeys::Gamepad_FaceButton_Bottom))
    {
        IsEvading = false;
        AnimInst->SetIgnoreRootMotion(IsEvading);
        GetCapsuleComponent()->SetCollisionResponseToChannel(
            ECC_GameTraceChannel5,
            ECR_Ignore);
    }
    else if(RightInput != 0.f || ForwardInput != 0.f)
    {
        IsEvading = false;
        AnimInst->SetIgnoreRootMotion(IsEvading);
	}
    StopAtk();
}

// パリィ状態を有効にし、後続処理を受け付ける状態にする。
void AMyPlayer::EnableExtraParrying()
{
    StopAtk();

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC->IsInputKeyDown(EKeys::SpaceBar) ||
        PC->IsInputKeyDown(EKeys::Gamepad_FaceButton_Right))
    {
        IsParrying = false;
        AnimInst->SetIgnoreRootMotion(IsParrying);
    }
}

// 死亡・被弾・回避・パリィ状態を確認し、新しい攻撃を開始できるか判定する。
bool AMyPlayer::AtkCheck()
{
    if (AnimInst->IsAttacking == true)
    {
        return true;
    }

    if (AnimInst->IsAttackingStrong == true)
    {
		return true;
    }

	return false;
}

// 再生中の攻撃と武器判定を停止し、攻撃に関する状態を初期化する。
void AMyPlayer::StopAtk()
{
    if (AMySword* Sword = Cast<AMySword>(SwordComponent->GetChildActor()))
    {
        Sword->EndTraceAttack();
    }
    AtkQueue.Empty();
    AnimInst->IsAttacking = false;
    AnimInst->IsAttackingStrong = false;
    LightAtkCnt = 0;
    HeavyAtkCnt = 5;
    ComboCnt = 0;
    canCombo = false;
    IsEvading = false;
	IsParrying = false;

    AttributeSet->SetAtkPower(PlayerAtkPower);

    AnimInst->Montage_Stop(0.1f, AttackMontage);
    AnimInst->Montage_Stop(0.1f, HeavyAttackMontage);
    AnimInst->Montage_Stop(0.1f, ComboAttackMontage);
}

// 現在入力方向を取得し、呼び出し元へ返す。
FVector AMyPlayer::GetCurrentInputDirection() const
{
    FRotator ControlRot = Controller->GetControlRotation();
    FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

    FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
    FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

    FVector Dir = Forward * ForwardInput + Right * RightInput;

	return Dir;
}

// 回避中のプレイヤー姿勢を使用し、残像アクターを現在位置へ生成する。
void AMyPlayer::SpawnAfterImage()
{
    AMyGhostActor* Ghost = GetWorld()->SpawnActor<AMyGhostActor>(
        GhostClass,
        GetMesh()->GetComponentTransform()
    );

    if (!Ghost) return;

    if(GhostTrailEffect)
    {
        GhostTrailEffect->Activate(true);
    }

    Ghost->Mesh->SetWorldTransform(GetMesh()->GetComponentTransform());

    Ghost->Mesh->SetLeaderPoseComponent(GetMesh());
    Ghost->Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
    Ghost->Mesh->Stop();
    Ghost->Mesh->bPauseAnims = true;
    Ghost->Mesh->SetComponentTickEnabled(false);

    Ghost->SetLifeSpan(0.1f);
}

// 衝突相手とダメージ量を確認し、プレイヤーへ通常の衝突ダメージを適用する。
void AMyPlayer::CalCollisionDamage(float DamageAmount, AActor* OtherActor)
{
    UEffectSubsystem* EffectSubsystem = GetWorld()->GetSubsystem<UEffectSubsystem>();
    if (EffectSubsystem)
    {
		EffectSubsystem->SpawnHitFX(GetActorLocation(), FRotator::ZeroRotator);
    }

    if (UGameAudioSubsystem* AudioSubsystem =
        GetGameInstance()->GetSubsystem<UGameAudioSubsystem>())
    {
        AudioSubsystem->PlayHitSound(GetActorLocation());
    }

    if (UCombatFeedbackSubsystem* CombatFeedbackSubsystem = GetWorld()->GetSubsystem<UCombatFeedbackSubsystem>())
    {
        CombatFeedbackSubsystem->PlayHitStop(HitStopThreshhold);
        CombatFeedbackSubsystem->PlayCameraShake(CameraShakeClass, CamerashakeScale);
    }

    float damage = DamageAmount * (100.0f / (100.0f + AttributeSet->GetDef()));

    AttributeSet->SetHp(FMath::Clamp(AttributeSet->GetHp() - damage, 0.f, PlayerMaxHP));
    SetHP(AttributeSet->GetHp() - damage);

    if(AttributeSet->GetHp() <= 0.f)
    {
        Death();
	}

    if (AnimInst->IsAttacking == true || AnimInst->IsAttackingStrong == true)
    {
        return;
    }
    StopAtk();
}

// 生存状態を解除し、移動・衝突・戦闘を停止した後に死亡演出を開始する。
void AMyPlayer::Death()
{
    StopAtk();

    IsAlive = false;

    StartDeathCamera();

    AnimInst->StopAllMontages(0.0f);
    AnimInst->SetIgnoreRootMotion(true);
    AnimInst->Montage_Play(DeathAnimMontage);
    AnimInst->Montage_JumpToSection("Death0");


    GetCharacterMovement()->StopMovementImmediately();
    GetCharacterMovement()->DisableMovement();

    if (PlayerLockonWidgetInstance)
    {
        PlayerLockonWidgetInstance->HideLockOnMarker();
    }
}

// 死亡演出用カメラを生成し、プレイヤーの視点を死亡演出へ切り替える。
void AMyPlayer::StartDeathCamera()
{
    UWorld* World = GetWorld();

    if (!IsValid(World))
    {
        return;
    }

    APlayerController* PC =
        World->GetFirstPlayerController();

    if (!IsValid(PC) ||
        !IsValid(PC->PlayerCameraManager))
    {
        return;
    }

    const FVector CurrentCameraLocation =
        PC->PlayerCameraManager->GetCameraLocation();

    const FRotator CurrentCameraRotation =
        PC->PlayerCameraManager->GetCameraRotation();

    FActorSpawnParameters Params;

    ADeathCameraActor* DeathCamera =
        World->SpawnActor<ADeathCameraActor>(
            ADeathCameraActor::StaticClass(),
            CurrentCameraLocation,
            CurrentCameraRotation,
            Params
        );

    if (!IsValid(DeathCamera))
    {
        return;
    }

    DeathCamera->SetTarget(GetMesh());

    PC->SetViewTargetWithBlend(
        DeathCamera,
        0.3f,
        VTBlend_Cubic
    );

    UGameFlowSubsystem* GameFlowSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UGameFlowSubsystem>();
    if (GameFlowSubsystem)
    {
        GameFlowSubsystem->ShowGameOver();
    }
}

// プレイヤーのギブアップ入力を受け取り、通常の死亡処理へ移行する。
void AMyPlayer::GiveUp()
{
    StopAtk();

    IsAlive = false;

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);


    GetCharacterMovement()->StopMovementImmediately();
    GetCharacterMovement()->DisableMovement();
}

// ラグドール移行後にアニメーション更新を停止し、物理姿勢だけを維持する。
void AMyPlayer::DisableAnimationAfterRagdoll()
{
    if (USkeletalMeshComponent* MeshComp = GetMesh())
    {
        MeshComp->SetAnimationMode(EAnimationMode::AnimationCustomMode);
        MeshComp->SetEnableAnimation(false);
    }
}

// 能力システムを通して、指定したダメージ量を対象の体力へ適用する。
void AMyPlayer::ApplyDamageToTarget(float DamageAmount, AActor* TargetActor)
{
    if (!TargetActor || !DamageEffectClass)
    {
        return;
    }

    UAbilitySystemComponent* TargetASC =
        UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

    if (!TargetASC)
    {
        return;
    }

    FGameplayEffectContextHandle Context = TargetASC->MakeEffectContext();
    Context.AddSourceObject(this);

    FGameplayEffectSpecHandle SpecHandle =
        TargetASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, Context);

    if (!SpecHandle.IsValid())
    {
        return;
    }

    SpecHandle.Data->SetSetByCallerMagnitude(
        FGameplayTag::RequestGameplayTag(TEXT("Data.Damage")),
        DamageAmount
    );

    TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

// 雷効果の実行条件を確認し、対象へ効果を適用する。
void AMyPlayer::ApplyLightningEffect(AActor* TargetActor)
{
	ApplyEffectToTarget(TargetActor, ShockEffectClass);
    ApplyDamageToTarget(AttributeSet->GetThunderDmg(), TargetActor);
}

// 風効果の実行条件を確認し、対象へ効果を適用する。
void AMyPlayer::ApplyWindEffect(AActor* TargetActor)
{
    ApplyEffectToTarget(TargetActor, WindEffectClass);
    ApplyDamageToTarget(AttributeSet->GetWindDmg(), TargetActor);
}

// 炎効果の実行条件を確認し、対象へ効果を適用する。
void AMyPlayer::ApplyFireEffect(AActor* TargetActor)
{
    ApplyEffectToTarget(TargetActor, FireEffectClass);
    ApplyDamageToTarget(AttributeSet->GetFireDmg(), TargetActor);
}

// 指定されたゲーム効果を生成し、対象の能力システムへ適用する。
void AMyPlayer::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> EffectClass)
{
    if (!TargetActor || !EffectClass)
    {
        return;
    }

    UAbilitySystemComponent* TargetASC =
        UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

    if (!TargetASC)
    {
        return;
    }

    FGameplayEffectContextHandle Context = TargetASC->MakeEffectContext();
    Context.AddSourceObject(this);

    FGameplayEffectSpecHandle SpecHandle =
        TargetASC->MakeOutgoingSpec(EffectClass, 1.0f, Context);

    if (SpecHandle.IsValid())
    {
        TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    }
}

// 属性攻撃に必要なスタミナを消費し、自動回復を一定時間停止する。
void AMyPlayer::ConsumeStamina(float Cost)
{
    if (!AbilitySystemComp || !ConsumeStaminaEffectClass)
    {
        return;
    }

    if (AttributeSet->GetSp() < Cost)
    {
        IsStaminaEnough = false;
        return;
    }

    FGameplayEffectContextHandle Context = AbilitySystemComp->MakeEffectContext();
    Context.AddSourceObject(this);

    FGameplayEffectSpecHandle SpecHandle =
        AbilitySystemComp->MakeOutgoingSpec(ConsumeStaminaEffectClass, 1.f, Context);

    if (!SpecHandle.IsValid())
    {
        return;
    }

    SpecHandle.Data->SetSetByCallerMagnitude(
        FGameplayTag::RequestGameplayTag(TEXT("Data.StaminaCost")),
        -Cost
    );

    AbilitySystemComp->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	SetSP(AttributeSet->GetSp() - Cost);
    BlockStaminaRegen(BlockStaminaRegenDelay);
}

// 攻撃スタミナを取得し、呼び出し元へ返す。
float AMyPlayer::GetElementAttackStaminaCost(EAttackAttribute Attr) const
{
    switch (Attr)
    {
    case EAttackAttribute::Fire:
        return FireSP;

    case EAttackAttribute::Lightning:
        return LightningSP;

    case EAttackAttribute::Wind:
        return WindSP;

    default:
        return 0.f;
    }
}

// 既存のタイマーを更新し、指定時間だけスタミナの自動回復を停止する。
void AMyPlayer::BlockStaminaRegen(float Delay)
{
    CanRegenStamina = false;

    GetWorldTimerManager().ClearTimer(StaminaRegenDelayHandle);
    GetWorldTimerManager().SetTimer(
        StaminaRegenDelayHandle,
        this,
        &AMyPlayer::EnableStaminaRegen,
        Delay,
        false
    );
}

// 回復待機時間の終了後、スタミナの自動回復を再び許可する。
void AMyPlayer::EnableStaminaRegen()
{
    CanRegenStamina = true;
}

// 回復待機状態と最大値を確認し、経過時間に応じてスタミナを回復する。
void AMyPlayer::RegenerateStamina(float DeltaTime)
{
    if (!CanRegenStamina || !AttributeSet)
    {
        return;
    }

    const float CurrentSp = AttributeSet->GetSp();
    const float MaxSp = AttributeSet->GetMaxSp();
    const float RegenRate = AttributeSet->GetSpRegenRate();

    if (CurrentSp < MaxSp)
    {
        const float NewSp = FMath::Clamp(CurrentSp + RegenRate * DeltaTime, 0.f, MaxSp);
        AttributeSet->SetSp(NewSp);
		SetSP(AttributeSet->GetSp());
    }

    if (AttributeSet->GetSp() >= GetElementAttackStaminaCost(CurrentAttackAttribute))
    {
		IsStaminaEnough = true;
    }
}


// パリィ状態を取得し、呼び出し元へ返す。
bool AMyPlayer::GetIsParrying() const
{
    return CapsuleParry->GetGenerateOverlapEvents();
}

// パリィ成功状態・ヒットストップ・効果音・視覚効果をまとめて適用する。
void AMyPlayer::SetParrying(FVector FXLocation)
{
    if (UEffectSubsystem* EffectSubsystem = GetWorld()->GetSubsystem<UEffectSubsystem>())
    {
        EffectSubsystem->SpawnParryFX(FXLocation, FRotator::ZeroRotator);
        EffectSubsystem->SpawnShockwaveFX(FXLocation, FRotator::ZeroRotator);
    }

    if (UCombatFeedbackSubsystem* CombatFeedbackSubsystem = GetWorld()->GetSubsystem<UCombatFeedbackSubsystem>())
    {
        CombatFeedbackSubsystem->PlayHitStop(HitStopThreshhold);
        CombatFeedbackSubsystem->PlayCameraShake(CameraShakeClass, CamerashakeScale);
    }

    if (UEffectActorSubsystem* Pool = GetWorld()->GetSubsystem<UEffectActorSubsystem>())
    {
        FTransform SpawnTransform;
        SpawnTransform.SetLocation(GetActorLocation());

        Pool->AcquireActor(ShockwaveActorClass, SpawnTransform, GetActorForwardVector());
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("EffectActorSubsystem not found!"));
    }

    StopAtk();
    AnimInst->Montage_Play(ParryMontage);
    AnimInst->Montage_JumpToSection(TEXT("ParrySuccess"));

    if (UGameAudioSubsystem* AudioSubsystem =
        GetGameInstance()->GetSubsystem<UGameAudioSubsystem>())
    {
        AudioSubsystem->PlayParryingSound(FXLocation);
    }
}

// 現在の状態に応じて最適対象の取得またはロックオン解除を実行する。
void AMyPlayer::ToggleLockOn()
{
    if (LockOnComponent->GetLockedTarget())
    {
        ClearLockOn();
        return;
    }

    LockOnComponent->FindBestTarget();

    if (LockOnComponent->GetLockedTarget())
    {
        GetCharacterMovement()->bOrientRotationToMovement = false;
    }
}

// 現在の対象より画面左側にいる候補を検索し、最適な対象へ切り替える。
void AMyPlayer::SwitchLockOnLeft()
{
    if (!LockOnComponent->GetLockedTarget())
    {
        ToggleLockOn();
        return;
    }

    LockOnComponent->FindSideTarget(false);
}

// 現在の対象より画面右側にいる候補を検索し、最適な対象へ切り替える。
void AMyPlayer::SwitchLockOnRight()
{
    if (!LockOnComponent->GetLockedTarget())
    {
        ToggleLockOn();
        return;
    }

    LockOnComponent->FindSideTarget(true);
}

// 対象参照を解除し、回転設定と照準表示を通常状態へ戻す。
void AMyPlayer::ClearLockOn()
{
    IsLockingOn = false;
    LockOnComponent->ClearLockOn();
}

// ゲームを一時停止し、入力方式と一時停止画面の表示状態を切り替える。
void AMyPlayer::Pause()
{
    UGameFlowSubsystem* GameFlowSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UGameFlowSubsystem>();
    if (GameFlowSubsystem)
    {
        GameFlowSubsystem->ShowPause();
    }
}
