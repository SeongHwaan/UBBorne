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

	BCharacter = Cast<AHunterCharacter>(GetCharacter());

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

	}
}

void ABBPlayerController::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	BCharacter->Move(MovementVector);
}

void ABBPlayerController::Look(const FInputActionValue& Value)
{
	const FVector2D	LookAxisVector = Value.Get<FVector2D>();
	BCharacter->Look(LookAxisVector);
}

void ABBPlayerController::StartJump()
{
}

void ABBPlayerController::StopJump()
{
}

void ABBPlayerController::LockOn()
{
	bool bLockCheck = BCharacter->GetIsLockOn();
    if (!bLockCheck)
    {
        BCharacter->LockOn();
        BCharacter->StopSprinting();
    }
	else
		BCharacter->LockOff();
}

void ABBPlayerController::Dodge()
{
	BCharacter->Dodging();
}

void ABBPlayerController::DodgeEnd()
{
    BCharacter->StopDodging();
}

void ABBPlayerController::StartSprint()
{
    bool bLockCheck = BCharacter->GetIsLockOn();
    if (bLockCheck)
    {
        BCharacter->LockOff();
    }
}

void ABBPlayerController::Sprinting()
{
    bool bLockCheck = BCharacter->GetIsLockOn();
    if (!bLockCheck)
    {
        BCharacter->Sprinting();
    }
}

void ABBPlayerController::StopSprint()
{
	BCharacter->StopSprinting();
}

void ABBPlayerController::LightAttack()
{
    BCharacter->LightAttack();
}

void ABBPlayerController::HeavyAttack()
{
    BCharacter->HeavyAttack();
}

void ABBPlayerController::WeaponChange()
{
    BCharacter->WeaponChange();
}
