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
#include "SingletonResourceManager.h"

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
    ECurrentActionType = EActionType::None;


    RightWeaponSocket = FName(TEXT("hand_rSocket"));
    WeaponManager = CreateDefaultSubobject<UWeaponManager>(TEXT("WeaponManager"));
}

void AHunterCharacter::BeginPlay()
{
    Super::BeginPlay();
    //무기 매니저를 통해서 사용할 무기 이름 넘김
    //그 무기 이름에 따라 instance 만들어 관리
    // 그걸 가져와 사용하기
    // 인벤토리는 정보만으로 관리 >> 정보에서 이름을 가져와 setRightWeapon
    //추후 아이템 던지는 것과 수혈액 기능 추가

    SetRightWeapon(RWeapon1, TEXT("SawCleaver"));
    CurrentRWeapon = RWeapon1;
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
        //Modify this
        //SetMovementState(EMovementState::None);
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
    ResourceManager = USingletonResourceManager::Get();
    ResourceManager->Initialize(HAnim, WeaponDataTable);
}

void AHunterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AHunterCharacter::Move(const FVector2D& Vector)
{

    if (HAnim->IsAnyMontagePlaying())
        return;

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
    {
        HAnim->PlayBackstepMontage();
        ECurrentMovementState = EMovementState::Backstep;
        SetMovementState(ECurrentMovementState);
    }
    else if (bIsLockOn)
    {
        HAnim->PlayLockOnDodgeMontage();
        ECurrentMovementState = EMovementState::Dodge;
        SetMovementState(ECurrentMovementState);
    }
    else
    {
        HAnim->PlayRollMontage();
        ECurrentMovementState = EMovementState::Roll;
        SetMovementState(ECurrentMovementState);
    }
}

void AHunterCharacter::StopDodging()
{
    bIsDodging = false;
}

bool AHunterCharacter::GetHasMovementInput() 
{
    return bHasMovementInput;
}

float AHunterCharacter::GetInputChangeRate()
{
    return InputChangeRate;
}

float AHunterCharacter::GetInputIntensity() 
{
    return InputIntensity;
}

bool AHunterCharacter::GetIsSprinting() 
{
    return bIsSprinting;
}

bool AHunterCharacter::GetIsDodging() 
{
    return bIsDodging;
}

float AHunterCharacter::GetDirectionAngle()
{
    return MovementDirectionAngle;
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
    auto WeaponInstance = WeaponManager->SetRWeapon(FName(TEXT("SawCleaver")));
    if (WeaponInstance)
    {
        NewWeapon->SetWeaponInstance(WeaponInstance);
        NewWeapon->SetWeaponMesh();
    }
    NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, RightWeaponSocket);
    NewWeapon->SetOwner(this);
    RightWeapon = NewWeapon;
}

void AHunterCharacter::LightAttack()
{
    //CurrentRWeaponState->LightAttack(this);
    ECurrentActionType = EActionType::LightAttack;
    CurrentMovementState->HandleAction(this, ECurrentActionType);
}

void AHunterCharacter::HeavyAttack()
{
    //CurrentRWeaponState->HeavyAttack(this);
    ECurrentActionType = EActionType::HeavyAttack;
    CurrentMovementState->HandleAction(this, ECurrentActionType);
}

void AHunterCharacter::WeaponChange()
{
    //CurrentRWeaponState->WeaponChange(this);
    ECurrentActionType = EActionType::WeaponChange;
    CurrentMovementState->HandleAction(this, ECurrentActionType);
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

void AHunterCharacter::NoneState::Move(AHunterCharacter* Chr)
{
    Chr->targetSpeed = 0.0f;
}

void AHunterCharacter::NoneState::HandleAction(AHunterCharacter* Chr, EActionType Action)
{
    auto& Anim = Chr->HAnim;
    switch (Action) {
    case EActionType::None:
        // NoneState에서는 이동하지 않음
        break;
    case EActionType::LightAttack:
        // 가벼운 공격 로직
        break;
    case EActionType::HeavyAttack:
        // 무거운 공격 로직
        break;
    case EActionType::WeaponChange:
        // 무기 교체 로직
        break;
    }
}

void AHunterCharacter::WalkState::Move(AHunterCharacter* Chr)
{
    Chr->targetSpeed = 0.14f;
}

void AHunterCharacter::WalkState::HandleAction(AHunterCharacter* Chr, EActionType Action)
{
}

void AHunterCharacter::RunState::Move(AHunterCharacter* Chr)
{
    Chr->targetSpeed = 0.42f;
}

void AHunterCharacter::RunState::HandleAction(AHunterCharacter* Chr, EActionType Action)
{
}

void AHunterCharacter::SprintState::Move(AHunterCharacter* Chr)
{
    Chr->targetSpeed = 0.65f;
}

void AHunterCharacter::SprintState::HandleAction(AHunterCharacter* Chr, EActionType Action)
{
}

void AHunterCharacter::RollState::HandleAction(AHunterCharacter* Chr, EActionType Action)
{
    auto Instance = Chr->CurrentRWeapon->GetWeaponInstance();
    switch (Action) 
    {
    case EActionType::None:
        break;
    case EActionType::LightAttack:
        //Use weapon state later
        Instance->LightAttack();
        break;
    case EActionType::HeavyAttack:
        break;
    case EActionType::WeaponChange:
        break;
    }
}

void AHunterCharacter::DodgeState::HandleAction(AHunterCharacter* Chr, EActionType Action)
{
}


void AHunterCharacter::BackstepState::HandleAction(AHunterCharacter* Chr, EActionType Action)
{
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
