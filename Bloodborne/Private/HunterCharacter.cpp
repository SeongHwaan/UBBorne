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

    CurrentMovementState = noneState.get();
    ECurrentMovementState = EMovementState::None;

    RightWeaponSocket = FName(TEXT("hand_rSocket"));


    CurrentRWeaponState = SawCleaver.get();
}

void AHunterCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (WeaponClass)
    {
        auto Weapon = GetWorld()->SpawnActor<ABBWeapon>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator);
        SetRightWeapon(Weapon);
    }
}

void AHunterCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (GetCharacterMovement()->GetCurrentAcceleration().Size() != 0)
    {
        bHasMovementInput = true;
    }
    else
    {
        bHasMovementInput = false;
        LastChangeCheckTime = GetWorld()->GetTimeSeconds();
        SetMovementState(EMovementState::None);
        CurrentMovementState = noneState.get();
    }

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
    HAnim = Cast<UHunterAnimInstance>(GetMesh()->GetAnimInstance());

    HAnim->OnMontageEnded.AddDynamic(this, &AHunterCharacter::OnAttackMontageEnded);
    HAnim->OnNextAttackCheck.AddLambda([this]() -> void {
        CanNextCombo = false;

        if (IsComboInputOn)
        {
            AttackStartComboState();
            HAnim->JumpToLightShortAttackMontageSection(CurrentCombo);
        }
        });
}

void AHunterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AHunterCharacter::Move(const FVector2D& Vector)
{
    FVector2D InputDirection = Vector;
    InputDirection.Normalize();

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

    const FRotator Rotation = GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);
    AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), InputDirection.Y * targetSpeed);
    AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), InputDirection.X * targetSpeed);

    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastChangeCheckTime >= ChangeCheckInterval)
    {
        InputChangeRate = FMath::Abs(InputIntensity - PreviousInputIntensity);
        PreviousInputIntensity = InputIntensity;
        LastChangeCheckTime = CurrentTime;
    }

    const FVector VelocityDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) * InputDirection.Y + FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) * InputDirection.X;
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

void AHunterCharacter::Sprinting()
{
    bIsSprinting = true;
    this->LockOff();
}

void AHunterCharacter::StopSprinting()
{
    bIsSprinting = false;
}

void AHunterCharacter::Dodging()
{
    bIsDodging = true;
    if (!bHasMovementInput)
        HAnim->PlayBackstepMontage();
    else if (bIsLockOn)
        HAnim->PlayLockOnDodgeMontage();
    else
        HAnim->PlayRollMontage();
}

void AHunterCharacter::StopDodging()
{
    bIsDodging = false;
}

bool AHunterCharacter::GetHasMovementInput() const
{
    return bHasMovementInput;
}

float AHunterCharacter::GetInputChangeRate() const
{
    return InputChangeRate;
}

float AHunterCharacter::GetInputIntensity() const
{
    return InputIntensity;
}

bool AHunterCharacter::GetIsSprinting() const
{
    return bIsSprinting;
}

bool AHunterCharacter::GetIsDodging() const
{
    return bIsDodging;
}

float AHunterCharacter::GetDirectionAngle() const
{
    return MovementDirectionAngle;
}

EMovementState AHunterCharacter::GetMovementState() const
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

void AHunterCharacter::SetRightWeapon(ABBWeapon* NewWeapon)
{
    if (CurrentWeapon != nullptr)
	{
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	if (NewWeapon != nullptr)
	{
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, RightWeaponSocket);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;
	}
}

void AHunterCharacter::LightAttack()
{
    CurrentRWeaponState->LightAttack(this);
}

void AHunterCharacter::HeavyAttack()
{
    CurrentRWeaponState->HeavyAttack(this);
}

void AHunterCharacter::WeaponChange()
{
    CurrentRWeaponState->WeaponChange(this);
}

void AHunterCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    IsAttacking = false;
    AttackEndComboState();
}

void AHunterCharacter::AttackStartComboState()
{
    CanNextCombo = true;
    IsComboInputOn = false;
    CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AHunterCharacter::AttackEndComboState()
{
    IsComboInputOn = false;
    CanNextCombo = false;
    CurrentCombo = 0;
}

void AHunterCharacter::MovementState::Move(AHunterCharacter* Chr)
{
}

void AHunterCharacter::NoneState::Move(AHunterCharacter* Chr)
{
    Chr->targetSpeed = 0.0f;
}

void AHunterCharacter::WalkState::Move(AHunterCharacter* Chr)
{
    Chr->targetSpeed = 0.14f;
}

void AHunterCharacter::RunState::Move(AHunterCharacter* Chr)
{
    Chr->targetSpeed = 0.42f;
}

void AHunterCharacter::SprintState::Move(AHunterCharacter* Chr)
{
    Chr->targetSpeed = 0.65f;
}

void AHunterCharacter::SawCleaverState::LightAttack(AHunterCharacter* Chr)
{
    auto& Anim = Chr->HAnim;
    if (Chr->IsAttacking)
    {
        if (Chr->CanNextCombo)
        {
            Chr->IsComboInputOn = true;
        }
    }
    else
    {
        Chr->AttackStartComboState();
        Anim->PlayLightShortAttackMontage();
        Anim->JumpToLightShortAttackMontageSection(Chr->CurrentCombo);
        Chr->IsAttacking = true;
    }
}

void AHunterCharacter::SawCleaverState::HeavyAttack(AHunterCharacter* Chr)
{
}

void AHunterCharacter::SawCleaverState::WeaponChange(AHunterCharacter* Chr)
{
}

void AHunterCharacter::SawCleaverState::AttackStartComboState(AHunterCharacter* Chr)
{
}

void AHunterCharacter::SawCleaverState::AttackEndComboState(AHunterCharacter* Chr)
{
}
