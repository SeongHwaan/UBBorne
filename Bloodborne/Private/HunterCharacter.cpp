// Fill out your copyright notice in the Description page of Project Settings.


#include "HunterCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputActionValue.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "HunterAnimInstance.h"
#include "BBWeapon.h"
#include "WeaponManager.h"
#include "WeaponInstance.h"
#include "BBGameInstance.h"

AHunterCharacter::AHunterCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->InitCapsuleSize(42.f, 90.0f);

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input... not working on root motion
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f); // ...at this rotation rate
    GetCharacterMovement()->JumpZVelocity = 700.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 1000.f;
    GetCharacterMovement()->MaxAcceleration = 10000.f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 400.0f;
    SpringArm->bUsePawnControlRotation = true; // Rotate the arm based on the controller

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    ECurrentMovementState = EMovementState::None;
    SetMovementState(ECurrentMovementState);

    ECurrentWeaponForm = EWeaponForm::Regular;

    RightWeaponSocket = FName(TEXT("hand_rSocket"));
    WeaponManager = CreateDefaultSubobject<UWeaponManager>(TEXT("WeaponManager"));
}

void AHunterCharacter::BeginPlay()
{
    Super::BeginPlay();

    UBBGameInstance* GameInstance = GetGameInstance<UBBGameInstance>();
    if (GameInstance)
    {
        //Set ResourceManager
        GameInstance->SetResourceManager(this);
        SetRightWeapon(RWeapon1, TEXT("SawCleaver"));
        CurrentRWeapon = RWeapon1;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("NoGmaeInstance"));
        return;
    }
}

void AHunterCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (TargetPawn != nullptr)
    {
        float InterpSpeed = 6.0f;
        FRotator TargetRotation = (TargetPawn->GetActorLocation() - GetActorLocation()).Rotation();
        FRotator NewRotation = FMath::RInterpTo(GetControlRotation(), TargetRotation, DeltaTime, InterpSpeed);
        GetController()->SetControlRotation(NewRotation);
        SetActorRotation(NewRotation);

        if (FVector::Dist(TargetPawn->GetActorLocation(), GetActorLocation()) > MaxLockOnDistance)
        {
            LockOff(); 
        }
    }
}

void AHunterCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    Anim = Cast<UHunterAnimInstance>(GetMesh()->GetAnimInstance());

    Anim->OnMontageStarted.AddDynamic(this, &AHunterCharacter::OnMontageStarted);
    Anim->OnMontageEnded.AddDynamic(this, &AHunterCharacter::OnMontageEnded);
    
    Anim->OnCanInput.AddLambda([this]() -> void {
        bCanInput = true;
        });
    Anim->OnNextActionCheck.AddLambda([this]() -> void {
        if (this->HasBufferedAction())
        {
            BufferedAction();
            BufferedAction = nullptr;
        }
        else
        {
            bIsAttacking = false;
            bCanNextAction = true;
            ResetMovementState();
        }
        });
    Anim->OnChargeStartCheck.AddLambda([this]() -> void {
        if (bIsCharging)
            bCanQuitCharge = true;
        else
            HeavyAttackEnd();
        });

    Anim->OnChargeEnd.AddLambda([this]() -> void {
        if (bIsCharging)
        {
            ChargeAttackEnd();
            bCanQuitCharge = false;
            bIsCharging = false;
            bChargeFinished = true;
        }
        });
    Anim->OnAttackEnd.AddLambda([this]() -> void {
        bChargeFinished = false;
        });
}

void AHunterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AHunterCharacter::Move(const FVector2D& Vector)
{
    bHasMovementInput = true;

    const FRotator Rotation = GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    InputDirection = Vector;
    InputDirection.Normalize();

    InputIntensity = Vector.Size();
    targetSpeed = 0.0f;

    if (bIsSprinting)
    {
        ECurrentMovementState = EMovementState::Sprint;
    }
    else if (InputIntensity >= MinimumInput && InputIntensity < MaximumInput)
    {
        ECurrentMovementState = EMovementState::Walk;
    }
    else if (InputIntensity >= MaximumInput)
    {
        ECurrentMovementState = EMovementState::Run;
    }
    else
    {
        //ECurrentMovementState = EMovementState::None;
        //SetMovementState(ECurrentMovementState);
        //return;
    }

    //State Pattern
    SetMovementState(ECurrentMovementState);
    CurrentMovementState->Move(this);

    if (Anim->IsAnyMontagePlaying())
    {
        //추후 통합 중단 로직 추가
        Anim->Montage_Stop(0.3f);
        Anim->OnAttackEnd.Broadcast();
    }

    AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), InputDirection.Y * targetSpeed);
    AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), InputDirection.X * targetSpeed);

    //float CurrentTime = GetWorld()->GetTimeSeconds();
    //if (CurrentTime - LastChangeCheckTime >= ChangeCheckInterval)
    //{
    //    InputChangeRate = FMath::Abs(InputIntensity - PreviousInputIntensity);
    //    PreviousInputIntensity = InputIntensity;
    //    LastChangeCheckTime = CurrentTime;
    //}
}

void AHunterCharacter::MoveEnd()
{
    InputDirection = FVector2D::ZeroVector;
    bHasMovementInput = false;
}

void AHunterCharacter::Look(const FVector2D& Vector)
{
    if (!bIsLockOn)
    {
        AddControllerPitchInput(Vector.Y);
        AddControllerYawInput(Vector.X);
    }
    else if (bIsLockOn)
    {
        //ChangeNearPawn By Direction
    }
}

void AHunterCharacter::Sprint()
{
    bIsSprinting = true;
    this->LockOff();
}

void AHunterCharacter::StopSprinting()
{
    bIsSprinting = false;
}

void AHunterCharacter::Dodge()
{
    if (!bHasMovementInput)
    {
        ECurrentMovementState = EMovementState::Backstep;
        SetMovementState(ECurrentMovementState);
        Anim->PlayBackstepMontage();
    }
    else if (bIsLockOn)
    {
        ECurrentMovementState = EMovementState::Dodge;
        SetMovementState(ECurrentMovementState);
        Anim->PlayLockOnDodgeMontage();
    }
    else
    {
        //Roll 방향 개선
        ECurrentMovementState = EMovementState::Roll;
        SetMovementState(ECurrentMovementState);
        Anim->PlayRollMontage();
    }
}

void AHunterCharacter::StopDodging()
{
}

void AHunterCharacter::ResetState()
{
    bIsAttacking = false;
    bCanInput = true;
    bCanNextAction = true;
    ResetMovementState();
}

bool AHunterCharacter::HasBufferedAction() const
{
    return static_cast<bool>(BufferedAction);
}


bool AHunterCharacter::GetbHasMovementInput() const
{
    return bHasMovementInput;
}

bool AHunterCharacter::GetIsSprinting() const
{
    return bIsSprinting;
}

bool AHunterCharacter::GetIsAttacking() const
{
    return bIsAttacking;
}

float AHunterCharacter::GetDirectionAngle()
{
    return MovementDirectionAngle;
}

void AHunterCharacter::SetDirectionAngle(FVector2D Vector)
{
    bHasMovementInput = true;

    auto DirectionVector = Vector;
    DirectionVector.Normalize();

    const FRotator Rotation = GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    const FVector VelocityDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) * DirectionVector.Y + 
        FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) * DirectionVector.X;
    const FVector ForwardDirection = GetActorForwardVector();
    const double Cos = ForwardDirection.Dot(VelocityDirection);
    const double Radian = FMath::Acos(Cos);
    const float Degree = FMath::RadiansToDegrees(Radian);
    const FVector Cross = FVector::CrossProduct(ForwardDirection, VelocityDirection);

    if (Cross.Z >= 0.0f)
        MovementDirectionAngle = Degree;
    else
        MovementDirectionAngle = -Degree;
}

float AHunterCharacter::GetLastDodgeAngle()
{
    return LastDodgeAngle;
}

void AHunterCharacter::SetLastDodgeAngle(float input)
{
    LastDodgeAngle = input;
}

bool AHunterCharacter::GetbCanInput() const
{
    return bCanInput;
}

bool AHunterCharacter::GetbCanNextAction() const
{
    return bCanNextAction;
}

void AHunterCharacter::SetbCanInput(bool input)
{
    bCanInput = input;
}

void AHunterCharacter::SetbCanNextAction(bool input)
{
    bCanNextAction = input;
}

EMovementState AHunterCharacter::GetMovementState() 
{
    return ECurrentMovementState;
}

void AHunterCharacter::SetMovementState(EMovementState NewState)
{
    switch (NewState)
    {
    case EMovementState::None:
        CurrentMovementState = noneState.get();
        break;
    case EMovementState::Walk:
        CurrentMovementState = walkState.get();
        break;
    case EMovementState::Run:
        CurrentMovementState = runState.get();
        break;
    case EMovementState::Sprint:
        CurrentMovementState = sprintState.get();
        break;
    case EMovementState::Dodge:
        CurrentMovementState = dodgeState.get();
        break;
    case EMovementState::Roll:
        CurrentMovementState = rollState.get();
        break;
    case EMovementState::Backstep:
        CurrentMovementState = backstepState.get();
        break;
    }
}

void AHunterCharacter::ResetMovementState()
{
    ECurrentMovementState = EMovementState::None;
    SetMovementState(ECurrentMovementState);
}


void AHunterCharacter::LockOn()
{
    TargetPawn = FindClosestPawn();
    if (TargetPawn != nullptr)
    {
        bIsLockOn = true;
        GetCharacterMovement()->bOrientRotationToMovement = false;
        GetCharacterMovement()->bUseControllerDesiredRotation = true;
        this->StopSprinting();
    }
}

void AHunterCharacter::LockOff()
{
    bIsLockOn = false;
    TargetPawn = nullptr;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->bUseControllerDesiredRotation = false;
}

bool AHunterCharacter::GetIsLockOn() const
{
    return bIsLockOn;
}

bool AHunterCharacter::IsTargetVisible(APawn* Target)
{
    FVector Start = Camera->GetComponentLocation();
    FVector End = Target->GetActorLocation();

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.AddIgnoredActor(Target);

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
    DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.0f, 0, 2.0f);

    if (bHit)
    {
        return false;
    }
    return true;
}

TObjectPtr<APawn> AHunterCharacter::FindClosestPawn()
{
    float ClosestDistance = MaxLockOnDistance;
    float FOV = Camera->FieldOfView;

    TObjectPtr<APawn> ClosestPawn = nullptr;

    for (TActorIterator<APawn> It(GetWorld()); It; ++It)
    {
        float Distance = FVector::Dist(It->GetActorLocation(), GetActorLocation());
        TObjectPtr<APawn> CurrentTargetPawn = *It;
        if (CurrentTargetPawn == this || Distance > MaxLockOnDistance)
            continue;

        FVector ToTarget = (CurrentTargetPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        FVector Forward = Camera->GetForwardVector();

        float DotProduct = FVector::DotProduct(Forward, ToTarget);
        float Angle = FMath::Acos(DotProduct) * (180.0f / PI);

        if (Angle > FOV)
        {
            continue;
        }

        if (!IsTargetVisible(CurrentTargetPawn))
        {
            continue;
        }

        if (Distance < ClosestDistance)
        {
            ClosestDistance = Distance;
            ClosestPawn = CurrentTargetPawn;
        }
    }

    if (ClosestPawn)
    {
        bIsLockOn = true;
        return ClosestPawn;
    }
    else
    {
        bIsLockOn = false;
        return nullptr;
    }
}

void AHunterCharacter::SetRightWeapon(TObjectPtr<class ABBWeapon>& RightWeapon, FName WeaponName)
{
    if (RightWeapon != nullptr)
    {
        RightWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        RightWeapon->Destroy();
        RightWeapon = nullptr;
    }

    auto NewWeapon = GetWorld()->SpawnActor<ABBWeapon>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator);
    if (auto WeaponInstance = WeaponManager->LoadRWeapon(WeaponName))
    {
        NewWeapon->SetWeaponInstance(WeaponInstance);
        NewWeapon->SetWeaponMeshComponent();
    }
    NewWeapon->Initialize();
    NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, RightWeaponSocket);
    NewWeapon->SetOwner(this);
    RightWeapon = NewWeapon;
}

ABBWeapon* AHunterCharacter::GetRightWeapon()
{
    return CurrentRWeapon;
}

EActionType AHunterCharacter::GetActionType() const
{
    return ECurrentActionType;
}

EWeaponForm AHunterCharacter::GetWeaponForm() const
{
    return ECurrentWeaponForm;
}

bool AHunterCharacter::GetbIsCharging() const
{
    return bIsCharging;
}

void AHunterCharacter::SetbIsCharging(bool input)
{
    bIsCharging = input;
}

bool AHunterCharacter::GetbCanQuitCharge() const
{
    return bCanQuitCharge;
}

void AHunterCharacter::SetbCanQuitCharge(bool input)
{
    bCanQuitCharge = input;
}

bool AHunterCharacter::GetbChargeFinished() const
{
    return bChargeFinished;
}

void AHunterCharacter::SetbChargeFinished(bool input)
{
    bChargeFinished = input;
}

void AHunterCharacter::LightAttack()
{
    bIsAttacking = true;
    ECurrentActionType = EActionType::LightAttack;
    CurrentMovementState->Attack(this);
    
    ResetMovementState();

    //ActionType도 추후 Reset
}

void AHunterCharacter::HeavyAttack()
{
    bIsAttacking = true;
    ECurrentActionType = EActionType::HeavyAttack;
    CurrentMovementState->Attack(this);
    
    ResetMovementState();
}

void AHunterCharacter::FormChange()
{
    ECurrentActionType = EActionType::FormChange;
    auto Form = GetWeaponForm();
    CurrentMovementState->Attack(this);

    if (ECurrentWeaponForm == EWeaponForm::Regular)
        ECurrentWeaponForm = EWeaponForm::Transformed;
    else
        ECurrentWeaponForm = EWeaponForm::Regular;

    ResetMovementState();
}

void AHunterCharacter::HeavyAttackEnd()
{
    auto Form = GetWeaponForm();
    CurrentRWeapon->HeavyEnd(Form);
}

void AHunterCharacter::ChargeAttackEnd()
{
    auto Form = GetWeaponForm();
    CurrentRWeapon->ChargeEnd(Form);
}

void AHunterCharacter::OnMontageStarted(UAnimMontage* Montage)
{
    bCanInput = false;
    bCanNextAction = false;
}

void AHunterCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
}

void AHunterCharacter::NoneState::Move(AHunterCharacter* Chr)
{
    Chr->targetSpeed = 0.0f;
}

void AHunterCharacter::NoneState::Attack(AHunterCharacter* Chr)
{
    CommonAttack(Chr);
}


void AHunterCharacter::WalkState::Move(AHunterCharacter* Chr)
{
    Chr->targetSpeed = 0.14f;
}

void AHunterCharacter::WalkState::Attack(AHunterCharacter* Chr)
{
    CommonAttack(Chr);
}


void AHunterCharacter::RunState::Move(AHunterCharacter* Chr)
{
    Chr->targetSpeed = 0.42f;
}

void AHunterCharacter::RunState::Attack(AHunterCharacter* Chr)
{
    CommonAttack(Chr);
}


void AHunterCharacter::SprintState::Move(AHunterCharacter* Chr)
{
    Chr->targetSpeed = 0.65f;
}

void AHunterCharacter::SprintState::Attack(AHunterCharacter* Chr)
{
    auto Weapon = Chr->GetRightWeapon();
    Weapon->SprintAttack(Chr->GetActionType(), Chr->GetWeaponForm());
}

void AHunterCharacter::RollState::Attack(AHunterCharacter* Chr)
{
    auto Weapon = Chr->GetRightWeapon();
    Weapon->RollAttack(Chr->GetActionType(), Chr->GetWeaponForm());
}

void AHunterCharacter::DodgeState::Attack(AHunterCharacter* Chr)
{
    auto Weapon = Chr->GetRightWeapon();
    Weapon->DodgeAttack(Chr->GetActionType(), Chr->GetWeaponForm(), Chr->GetLastDodgeAngle());
}

void AHunterCharacter::BackstepState::Attack(AHunterCharacter* Chr)
{
    auto Weapon = Chr->GetRightWeapon();
    Weapon->BackstepAttack(Chr->GetActionType(), Chr->GetWeaponForm());
}

void AHunterCharacter::MovementState::CommonAttack(AHunterCharacter* Chr)
{
    auto Weapon = Chr->GetRightWeapon();
    auto Action = Chr->GetActionType();
    auto Form = Chr->GetWeaponForm();

    switch (Action)
    {
    case EActionType::LightAttack:
        Weapon->LightCombo(Form);
        break;
    case EActionType::HeavyAttack:
        if (Chr->GetbChargeFinished() && Form == EWeaponForm::Transformed)
        {

            //ChargeEnd 이후에 끊기면 SetbChargeFinished가 계속 true가 되는 문제
            Chr->SetbChargeFinished(false);
            Weapon->HeavyAfterCharge();
        }
        else
            Weapon->HeavyStart(Form);
        break;
    case EActionType::FormChange:
        Weapon->FormChange(Form, Chr->GetIsAttacking());
        break;
    }
}

void AHunterCharacter::MovementState::WeaponChange(AHunterCharacter* Chr)
{
}