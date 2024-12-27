// Fill out your copyright notice in the Description page of Project Settings.


#include "HunterAnimInstance.h"
#include "BBPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Quat.h"
#include "Animation/AnimSequence.h"

UHunterAnimInstance::UHunterAnimInstance()
{
}

void UHunterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn))
		return;

	PCharacter = Cast<AHunterCharacter>(Pawn);

	HasMovementInput = PCharacter->GetHasMovementInput();
	InputChangeRate = PCharacter->GetInputChangeRate();
	InputIntensity = PCharacter->GetInputIntensity();
	IsLockOn = PCharacter->GetIsLockOn();
	IsSprinting = PCharacter->GetIsSprinting();
	IsDodging = PCharacter->GetIsDodging();

    MovementDirectionAngle = PCharacter->GetDirectionAngle();
    MovementState = PCharacter->GetMovementState();
}

void UHunterAnimInstance::PlayLockOnDodgeMontage()
{
    const float angle = MovementDirectionAngle;
    if (FMath::Abs(angle) <= 22.5)
        Montage_Play(Forward, 1.0f);
    else if (22.5 < angle && angle <= 67.5)
        Montage_Play(ForwardRight, 1.0f);
    else if (67.5 < angle && angle <= 112.5)
        Montage_Play(Right, 1.0f);
    else if (112.5 < angle && angle < 157.5)
        Montage_Play(BackwardRight, 1.0f);
    else if (FMath::Abs(angle) >= 157.5)
        Montage_Play(Backward, 1.0f);
    else if (-157.5 < angle && angle <= -112.5)
        Montage_Play(BackwardLeft, 1.0f);
    else if (-112.5 < angle && angle <= -67.5)
        Montage_Play(Left, 1.0f);
    else if (-67.5 < angle && angle < -22.5)
        Montage_Play(ForwardLeft, 1.0f);
}

void UHunterAnimInstance::PlayRollMontage()
{
    Montage_Play(Roll, 1.0f);
}

void UHunterAnimInstance::PlayBackstepMontage()
{
    Montage_Play(Backstep, 1.0f);
}

void UHunterAnimInstance::PlayLightShortAttackMontage()
{
    Montage_Play(ShortLightAttack1, 1.0f);
}

void UHunterAnimInstance::JumpToLightShortAttackMontageSection(int32 NewSection)
{
    Montage_JumpToSection(GetAttackMontageSectionName(NewSection), ShortLightAttack1);
}

void UHunterAnimInstance::PlayRollLightMontage()
{
    Montage_Play(RollLightAttack, 1.0f);
}

void UHunterAnimInstance::AnimNotify_AttackHitCheck()
{
    OnAttackHitCheck.Broadcast();
}

void UHunterAnimInstance::AnimNotify_NextAttackCheck()
{
    OnNextAttackCheck.Broadcast();
}

FName UHunterAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	//CHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
	return FName(*FString::Printf(TEXT("%d"), Section));
}


