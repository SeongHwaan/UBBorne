// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bloodborne.h"
#include "HunterCharacter.h"
#include "Animation/AnimInstance.h"
#include "HunterAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnNextAttatckCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);

UCLASS()
class BLOODBORNE_API UHunterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UHunterAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
    EMovementState MovementState = EMovementState::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
    TObjectPtr<class AHunterCharacter> PCharacter;

public:
    //Movement
    void PlayLockOnDodgeMontage();
    void PlayRollMontage();
    void PlayBackstepMontage();

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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge", Meta = (AllowPrivateAccess = true))
    TObjectPtr<UAnimMontage> Roll;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge", Meta = (AllowPrivateAccess = true))
    TObjectPtr<UAnimMontage> Backstep;

public:
    //Attack
    FOnNextAttatckCheckDelegate OnNextAttackCheck;
    FOnAttackHitCheckDelegate OnAttackHitCheck;
    void PlayLightShortAttackMontage();

    void JumpToLightShortAttackMontageSection(int32 NewSection);

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
    TObjectPtr<UAnimMontage> ShortLightAttack1;

    UFUNCTION()
    void AnimNotify_AttackHitCheck();

    UFUNCTION()
    void AnimNotify_NextAttackCheck();

    FName GetAttackMontageSectionName(int32 Section);
};
