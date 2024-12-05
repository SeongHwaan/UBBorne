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
}

void AHunterCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AHunterCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (GetCharacterMovement()->GetCurrentAcceleration().Size() != 0)
    {
        HasMovementInput = true;
        SetMovementState(EMovementState::None);
        CurrentMovementState = noneState.get();
    }
    else
    {
        HasMovementInput = false;
        LastChangeCheckTime = GetWorld()->GetTimeSeconds();
    }

    if (TargetPawn != nullptr)
    {
        float InterpSpeed = 10.0f;
        FRotator TargetRotation = (TargetPawn->GetActorLocation() - GetActorLocation()).Rotation();
        FRotator NewRotation = FMath::RInterpTo(GetControlRotation(), TargetRotation, DeltaTime, InterpSpeed);
        GetController()->SetControlRotation(NewRotation);

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

    if (IsSprinting)
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
    if (!IsLockOn)
    {
        AddControllerPitchInput(Vector.Y);
        AddControllerYawInput(Vector.X);
    }
    else if (IsLockOn)
    {
        //ChangeNearPawn By Direction
    }
}

void AHunterCharacter::Attack()
{
}

void AHunterCharacter::Sprinting()
{
    IsSprinting = true;
}

void AHunterCharacter::StopSprinting()
{
    IsSprinting = false;
}

void AHunterCharacter::Dodging()
{
    IsDodging = true;
    HAnim->PlayDodgeMontage();
}

void AHunterCharacter::StopDodging()
{
    IsDodging = false;
}

bool AHunterCharacter::GetHasMovementInput() const
{
    return HasMovementInput;
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
    return IsSprinting;
}

bool AHunterCharacter::GetIsDodging() const
{
    return IsDodging;
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
    IsLockOn = true;
    TargetPawn = FindClosestPawn();
    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void AHunterCharacter::LockOff()
{
    IsLockOn = false;
    TargetPawn = nullptr;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->bUseControllerDesiredRotation = false;
}

bool AHunterCharacter::GetIsLockOn() const
{
    return IsLockOn;
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
        IsLockOn = true;
        return ClosestPawn;
    }
    else
    {
        IsLockOn = false;
        return nullptr;
    }
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
