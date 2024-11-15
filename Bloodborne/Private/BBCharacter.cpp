// Fill out your copyright notice in the Description page of Project Settings.


#include "BBCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputActionValue.h"
	
ABBCharacter::ABBCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 90.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f); // ...at this rotation rate

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	//GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

// Called when the game starts or when spawned
void ABBCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABBCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetCharacterMovement()->GetCurrentAcceleration().Size() != 0)
		HasMovementInput = true;
	else
	{
		HasMovementInput = false;
		LastChangeCheckTime = GetWorld()->GetTimeSeconds();
	}
}

// Called to bind functionality to input
void ABBCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABBCharacter::Move(const FVector2D& Vector)
{
	InputIntensity = Vector.Size();
	float targetSpeed = 0.0f;

	if (InputIntensity >= 0.5f && InputIntensity < 0.9f)
	{
		targetSpeed = 0.15f;
	}
	else if (InputIntensity >= 0.9f)
	{ 
		targetSpeed = 0.36f;
	}
	FVector2D moveDirection = FVector2D(Vector.X, Vector.Y);  
	moveDirection.Normalize();  

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), moveDirection.Y * targetSpeed);
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), moveDirection.X * targetSpeed);

	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastChangeCheckTime >= ChangeCheckInterval)
	{
		InputChangeRate = FMath::Abs(InputIntensity - PreviousInputIntensity);
		PreviousInputIntensity = InputIntensity;
		LastChangeRateCheckTime = CurrentTime;
	}
}

void ABBCharacter::Look(const FVector2D& Vector)
{
	AddControllerPitchInput(Vector.Y);
	AddControllerYawInput(Vector.X);
}

bool ABBCharacter::GetHasMovementInput()
{
	return HasMovementInput;
}

float ABBCharacter::GetInputChangeRate()
{
	return InputChangeRate;
}

float ABBCharacter::GetInputIntensity()
{
	return InputIntensity;
}
