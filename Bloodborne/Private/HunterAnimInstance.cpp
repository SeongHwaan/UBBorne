// Fill out your copyright notice in the Description page of Project Settings.


#include "HunterAnimInstance.h"
#include "BBCharacter.h"
#include "BBPlayerController.h"
#include "Kismet/GameplayStatics.h"

UHunterAnimInstance::UHunterAnimInstance()
{
}

void UHunterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn))
		return;

	auto PCharacter = Cast<ABBCharacter>(Pawn);

	HasMovementInput = PCharacter->GetHasMovementInput();
	InputChangeRate = PCharacter->GetInputChangeRate();
	InputIntensity = PCharacter->GetInputIntensity();
}
