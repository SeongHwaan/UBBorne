// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bloodborne.h"
#include "Animation/AnimInstance.h"
#include "HunterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BLOODBORNE_API UHunterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UHunterAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	float InputIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	bool HasMovementInput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	float InputChangeRate;

};
