// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bloodborne.h"
#include "WeaponData.h"
#include "HunterCharacter.h"
#include "UObject/NoExportTypes.h"
#include "WeaponInstance.generated.h"

/**
 * 
 */
UCLASS()
class BLOODBORNE_API UWeaponInstance : public UObject
{
	GENERATED_BODY()

public:
    UWeaponInstance();

    void InitializeWeapon(FName RowName);

    TObjectPtr<USkeletalMesh> GetWeaponMesh() const;

    void SetWeaponMeshComp(USkeletalMeshComponent* Component);
    void SetAttackIndex(int input);

    virtual void PlayLightCombo(EWeaponForm Form);
    virtual void PlayHeavyStart(EWeaponForm Form);
    virtual void PlayHeavyEnd(EWeaponForm Form);
    virtual void PlayChargeEnd(EWeaponForm Form);
    virtual void PlayHeavyAfterCharge();
    virtual void PlayRollAttack(EActionType Action, EWeaponForm Form);
    virtual void PlayBackstepAttack(EActionType Action, EWeaponForm Form);
    virtual void PlayDodgeAttack(EActionType Action, EWeaponForm Form, float angle);
    virtual void PlayJumpAttack(EWeaponForm Form);
    virtual void PlaySprintAttack(EActionType Action, EWeaponForm Form);
    virtual void PlayFormChange(EWeaponForm Form, bool bIsAttacking);

    //Light에 의해 Index가 2 이상인 된 경우를 위해
    void CheckLeftRight();
    void NextLeftRight();
    void ResetState();

    void SetAnimData(FName Key);
    void PlayAttackAnim();

protected:
    // Don't think UPROPERTY() is necessary
    TObjectPtr<USkeletalMesh> WeaponMesh;
    FName WeaponName;
    FWeaponData* WeaponData;
    TMap<FName, FAttackAnimationData> LoadedWeaponAnimations;
    FAttackAnimationData* CurrAnimData;
    UAnimInstance* PlayerAnimInstance;
    USkeletalMeshComponent* WeaponMeshComp;

    //AttackIndex: 0 -> Right Start
    //AttackIndex: 1 -> Left Start
    int32 AttackIndex;
    bool bIsRight;
    FName MontageName;

};
