// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bloodborne.h"
#include "HunterCharacter.h"
#include "Animation/AnimInstance.h"
#include "HunterAnimInstance.generated.h"

UCLASS()
class BLOODBORNE_API UHunterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UHunterAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

    void PlayDodgeMontage();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	float InputIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	bool HasMovementInput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	float InputChangeRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	bool IsLockOn;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	bool IsSprinting;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	bool IsDodging;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	float MovementDirectionAngle;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
    EMovementState MovementState = EMovementState::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
    TObjectPtr<class AHunterCharacter> PCharacter;


    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge", Meta = (AllowPrivateAccess = true))
    TObjectPtr<UAnimMontage> Forward;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge", Meta = (AllowPrivateAccess = true))
    TObjectPtr<UAnimMontage> Backward;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge", Meta = (AllowPrivateAccess = true))
    TObjectPtr<UAnimMontage> Left;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge", Meta = (AllowPrivateAccess = true))
    TObjectPtr<UAnimMontage> Right;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge", Meta = (AllowPrivateAccess = true))
    TObjectPtr<UAnimMontage> ForwardLeft;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge", Meta = (AllowPrivateAccess = true))
    TObjectPtr<UAnimMontage> ForwardRight;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge", Meta = (AllowPrivateAccess = true))
    TObjectPtr<UAnimMontage> BackwardLeft;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge", Meta = (AllowPrivateAccess = true))
    TObjectPtr<UAnimMontage> BackwardRight;
};
