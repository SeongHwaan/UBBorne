﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bloodborne.h"
#include "HunterCharacter.h"
#include "Animation/AnimInstance.h"
#include "HunterAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnNextAttatckCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackEndedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnChargeStartCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnChargeEndedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnCanInputDelegate);
DECLARE_MULTICAST_DELEGATE(FOnFormAttackDelegate);

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
	bool HasMovementInput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pawn", Meta = (AllowPrivateAccess = true))
	bool IsLockOn;

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
    FOnCanInputDelegate OnCanInput;
    FOnNextAttatckCheckDelegate OnNextActionCheck;
    FOnAttackHitCheckDelegate OnAttackHitCheck;
    FOnAttackEndedDelegate OnAttackEnd;
    FOnChargeStartCheckDelegate OnChargeStartCheck;
    FOnChargeEndedDelegate OnChargeEnd;
    FOnFormAttackDelegate OnFormAttack;


private:

    UFUNCTION()
    void AnimNotify_CanInput();

    UFUNCTION()
    void AnimNotify_NextActionCheck();

    UFUNCTION()
    void AnimNotify_AttackHitCheck();

    UFUNCTION()
    void AnimNotify_AttackEnd();

    UFUNCTION()
    void AnimNotify_ChargeStartCheck();

    UFUNCTION()
    void AnimNotify_ChargeEnd();

    UFUNCTION()
    void AnimNotify_FormAttack();
};