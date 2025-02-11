﻿// Fill out your copyright notice in the Description page of Project Settings.


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
        return;
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
            bCanNextAction = true;
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
        }
        });
}

void AHunterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AHunterCharacter::Move(const FVector2D& Vector)
{
    InputDirection = Vector;
    InputDirection.Normalize();

    const FRotator Rotation = GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    InputIntensity = Vector.Size();
    targetSpeed = 0.0f;

    if (bIsSprinting)
    {
        ECurrentMovementState = EMovementState::Sprint;
    }
    else if (InputIntensity >= 0.5f && InputIntensity < 0.9f)
    {
        ECurrentMovementState = EMovementState::Walk;
    }
    else if (InputIntensity >= 0.9f)
    {
        ECurrentMovementState = EMovementState::Run;
    }
    else
    {
        ECurrentMovementState = EMovementState::None;
    }
    //State Pattern
    SetMovementState(ECurrentMovementState);
    CurrentMovementState->Move(this);

    if (Anim->IsAnyMontagePlaying())
    {
        Anim->Montage_Stop(0.3f);
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
    //MovementDirectionAngle = 0.0f;
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
    bIsDodging = true;

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
        ECurrentMovementState = EMovementState::Roll;
        SetMovementState(ECurrentMovementState);
        Anim->PlayRollMontage();
    }
}

void AHunterCharacter::StopDodging()
{
    bIsDodging = false;
}

bool AHunterCharacter::HasBufferedAction() const
{
    return static_cast<bool>(BufferedAction);
}


bool AHunterCharacter::GetbHasMovementInput() const
{
    return bHasMovementInput;
}

void AHunterCharacter::SetbHasMovementInput(bool input)
{
    bHasMovementInput = input;
}

bool AHunterCharacter::GetIsSprinting() const
{
    return bIsSprinting;
}

bool AHunterCharacter::GetIsDodging() const
{
    return bIsDodging;
}

float AHunterCharacter::GetDirectionAngle()
{
    return MovementDirectionAngle;
}

void AHunterCharacter::SetDirectionAngle(FVector2D Vector)
{
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

    UE_LOG(LogTemp, Warning, TEXT("Angle: %f"), MovementDirectionAngle);
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
    if (NewState == EMovementState::None)
        CurrentMovementState = noneState.get();
    else if (NewState == EMovementState::Walk)
        CurrentMovementState = walkState.get();
    else if (NewState == EMovementState::Run)
        CurrentMovementState = runState.get();
    else if (NewState == EMovementState::Sprint)
        CurrentMovementState = sprintState.get();
    else if (NewState == EMovementState::Dodge)
        CurrentMovementState = dodgeState.get();
    else if (NewState == EMovementState::Roll)
        CurrentMovementState = rollState.get();
    else if (NewState == EMovementState::Backstep)
        CurrentMovementState = backstepState.get();
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
    auto WeaponInstance = WeaponManager->LoadRWeapon(WeaponName);
    if (WeaponInstance)
    {
        NewWeapon->SetWeaponInstance(WeaponInstance);
        NewWeapon->SetWeaponMesh();
    }
    NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, RightWeaponSocket);
    NewWeapon->SetOwner(this);
    RightWeapon = NewWeapon;
}

ABBWeapon* AHunterCharacter::GetRightWeapon()
{
    return CurrentRWeapon;
}

UDataTable* AHunterCharacter::GetWeaponDataTable() const
{
    return WeaponDataTable;
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

void AHunterCharacter::LightAttack()
{
    ECurrentActionType = EActionType::LightAttack;
    CurrentMovementState->HandleAction(this);
}

void AHunterCharacter::HeavyAttack()
{
    ECurrentActionType = EActionType::HeavyAttack;
    CurrentMovementState->HandleAction(this);
}

void AHunterCharacter::WeaponChange()
{
    ECurrentActionType = EActionType::WeaponChange;
    CurrentMovementState->HandleAction(this);
}

void AHunterCharacter::HeavyAttackEnd()
{
    auto Instance = CurrentRWeapon->GetWeaponInstance();
    auto Form = GetWeaponForm();
    Instance->HeavyEnd(Form);
}

void AHunterCharacter::ChargeAttackEnd()
{
    auto Instance = CurrentRWeapon->GetWeaponInstance();
    auto Form = GetWeaponForm();
    Instance->ChargeEnd(Form);
}

void AHunterCharacter::OnMontageStarted(UAnimMontage* Montage)
{
    bCanInput = false;
    bCanNextAction = false;
    UE_LOG(LogTemp, Warning, TEXT("MontageStarted"));
}

void AHunterCharacter::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
}

void AHunterCharacter::NoneState::Move(AHunterCharacter* Chr)
{
    Chr->targetSpeed = 0.0f;
}
//switch 방식이 확장성은 좋으나 상태별로 함수를 많이 만들어야 하기에 가독성이 떨어지고 불편
//차라리 무기 상태나 공격 상태를 넘겨 if문으로 알아서 처리하도록
void AHunterCharacter::NoneState::HandleAction(AHunterCharacter* Chr)
{
    NormalAction(Chr);
}


void AHunterCharacter::WalkState::Move(AHunterCharacter* Chr)
{
    Chr->targetSpeed = 0.14f;
}

void AHunterCharacter::WalkState::HandleAction(AHunterCharacter* Chr)
{
    NormalAction(Chr);
}


void AHunterCharacter::RunState::Move(AHunterCharacter* Chr)
{
    Chr->targetSpeed = 0.42f;
}

void AHunterCharacter::RunState::HandleAction(AHunterCharacter* Chr)
{
    NormalAction(Chr);
}


void AHunterCharacter::SprintState::Move(AHunterCharacter* Chr)
{
    Chr->targetSpeed = 0.65f;
}

void AHunterCharacter::SprintState::HandleAction(AHunterCharacter* Chr)
{

}

void AHunterCharacter::RollState::HandleAction(AHunterCharacter* Chr)
{
    auto Instance = Chr->CurrentRWeapon->GetWeaponInstance();
    auto Action = Chr->GetActionType();
    auto Form = Chr->GetWeaponForm();

    Instance->RollAttack(Action, Form);
}

void AHunterCharacter::DodgeState::HandleAction(AHunterCharacter* Chr)
{
}


void AHunterCharacter::BackstepState::HandleAction(AHunterCharacter* Chr)
{
}

void AHunterCharacter::MovementState::NormalAction(AHunterCharacter* Chr)
{
    auto Instance = Chr->CurrentRWeapon->GetWeaponInstance();
    auto Action = Chr->GetActionType();
    auto Form = Chr->GetWeaponForm();

    switch (Action)
    {
    case EActionType::LightAttack:
        Instance->LightCombo(Form);
        break;
    case EActionType::HeavyAttack:
        Instance->HeavyStart(Form);
        break;
    case EActionType::WeaponChange:
        break;
    }
}




//void AHunterCharacter::SawCleaverState::LightAttack(AHunterCharacter* Chr)
//{
//    auto& Anim = Chr->HAnim;
//    if (Chr->IsAttacking)
//    {
//        if (Chr->CanNextCombo)
//        {
//            Chr->IsComboInputOn = true;
//        }
//    }
//    else
//    {
//        Chr->AttackStartComboState();
//        Anim->PlayLightShortAttackMontage();
//        Anim->JumpToLightShortAttackMontageSection(Chr->CurrentCombo);
//        Chr->IsAttacking = true;
//    }
//}