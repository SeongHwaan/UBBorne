// Fill out your copyright notice in the Description page of Project Settings.


#include "BBPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HunterCharacter.h"

//Use template if At least 2 Pawn would be used
ABBPlayerController::ABBPlayerController()
{
}

void ABBPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ABBPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
}

void ABBPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BBCharacter = Cast<AHunterCharacter>(GetCharacter());

	if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		//SubSystem->ClearAllMappings();
		SubSystem->AddMappingContext(DefaultContext, 0);
	}
}

void ABBPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
    EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABBPlayerController::Move);
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ABBPlayerController::MoveEnd);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABBPlayerController::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ABBPlayerController::StartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ABBPlayerController::StopJump);
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Completed, this, &ABBPlayerController::LockOn);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &ABBPlayerController::Dodge);
        EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Completed, this, &ABBPlayerController::DodgeEnd);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ABBPlayerController::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Ongoing, this, &ABBPlayerController::Sprinting);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ABBPlayerController::StopSprint);

        EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Started, this, &ABBPlayerController::LightAttack);
        EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &ABBPlayerController::HeavyAttackStart);
        EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Completed, this, &ABBPlayerController::HeavyAttackEnd);
        EnhancedInputComponent->BindAction(WeaponChangeAction, ETriggerEvent::Started, this, &ABBPlayerController::WeaponChange);

	}
}

void ABBPlayerController::Move(const FInputActionValue& Value)
{
    if (BBCharacter->GetbCanNextAction())
    {
        const FVector2D MovementVector = Value.Get<FVector2D>();
        BBCharacter->Move(MovementVector);
    }
}

void ABBPlayerController::MoveEnd()
{
    BBCharacter->MoveEnd();
}

void ABBPlayerController::Look(const FInputActionValue& Value)
{
	const FVector2D	LookAxisVector = Value.Get<FVector2D>();
	BBCharacter->Look(LookAxisVector);
}

void ABBPlayerController::StartJump()
{
}

void ABBPlayerController::StopJump()
{
}

void ABBPlayerController::LockOn()
{
	bool bLockCheck = BBCharacter->GetIsLockOn();
    if (!bLockCheck)
    {
        BBCharacter->LockOn();
        BBCharacter->StopSprinting();
    }
	else
		BBCharacter->LockOff();
}

void ABBPlayerController::Dodge()
{
	BBCharacter->Dodging();
}

void ABBPlayerController::DodgeEnd()
{
    BBCharacter->StopDodging();
}

void ABBPlayerController::StartSprint()
{
    bool bLockCheck = BBCharacter->GetIsLockOn();
    if (bLockCheck)
    {
        BBCharacter->LockOff();
    }
}

void ABBPlayerController::Sprinting()
{
    bool bLockCheck = BBCharacter->GetIsLockOn();
    if (!bLockCheck)
    {
        BBCharacter->Sprinting();
    }
}

void ABBPlayerController::StopSprint()
{
	BBCharacter->StopSprinting();
}

void ABBPlayerController::LightAttack()
{
    if (BBCharacter->GetbCanInput() && !BBCharacter->GetbCanNextAction())
    {
        BBCharacter->AddBufferAction(EActionType::LightAttack);
        return;
    }
    if (BBCharacter->GetbCanNextAction())
    {
        BBCharacter->ConsumeBufferedAction();
        BBCharacter->LightAttack();
        return;
    }
}

void ABBPlayerController::HeavyAttackStart()
{
    BBCharacter->SetbIsCharging(true);

    if (BBCharacter->GetbCanInput() && !BBCharacter->GetbCanNextAction())
    {
        BBCharacter->AddBufferAction(EActionType::HeavyAttack);
        return;
    }
    if (BBCharacter->GetbCanNextAction())
    {
        BBCharacter->ConsumeBufferedAction();
        BBCharacter->HeavyAttack();
        return;
    }
}

void ABBPlayerController::HeavyAttackEnd()
{
    BBCharacter->SetbIsCharging(false);

    //OnChargeStartCheck에서 IsCharging이 true인 경우 진입 가능
    if (BBCharacter->GetbCanQuitCharge())
    {
        BBCharacter->SetbCanQuitCharge(false);
        BBCharacter->HeavyAttackEnd();
    }

    UE_LOG(LogTemp, Warning, TEXT("Released"));
}

void ABBPlayerController::WeaponChange()
{
    BBCharacter->WeaponChange();
}
