// Fill out your copyright notice in the Description page of Project Settings.


#include "BBCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputActionValue.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include <Kismet/KismetMathLibrary.h>
	
ABBCharacter::ABBCharacter()
{
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
}

// Called to bind functionality to input
void ABBCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
