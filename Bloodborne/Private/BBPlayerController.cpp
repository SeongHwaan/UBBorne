// Fill out your copyright notice in the Description page of Project Settings.


#include "BBPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BBCharacter.h"

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

	BCharacter = Cast<ABBCharacter>(GetCharacter());

	if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		//SubSystem->ClearAllMappings();
		SubSystem->AddMappingContext(DefaultContext, 0);
	}
}

void ABBPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABBPlayerController::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABBPlayerController::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ABBPlayerController::StartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ABBPlayerController::StopJump);
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Completed, this, &ABBPlayerController::LockOn);
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

void ABBPlayerController::Attack()
{
}

void ABBPlayerController::LockOn()
{
	bool LockCheck = BCharacter->GetIsLockOn();
	if (!LockCheck)
		BCharacter->LockOn();
	else
		BCharacter->LockOff();
}
