﻿// Fill out your copyright notice in the Description page of Project Settings.

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

    const TObjectPtr<USkeletalMesh> GetWeaponMesh();

    void SetAttackIndex(int input);

    virtual void LightCombo(EWeaponForm Form);
    virtual void HeavyStart(EWeaponForm Form);
    virtual void HeavyEnd(EWeaponForm Form);
    virtual void ChargeEnd(EWeaponForm Form);
    virtual void RollAttack(EActionType Action, EWeaponForm Form);
    virtual void BackstepAttack(EActionType Action, EWeaponForm Form);
    virtual void DodgeAttack(EActionType Action, EWeaponForm Form) {};
    virtual void JumpAttack(EWeaponForm Form);
    virtual void SprintAttack(EActionType Action, EWeaponForm Form);
    virtual void WeaponChange() {};

    void ResetState();
    void PlayAttackAnim(FName Key);

protected:
    // Don't think UPROPERTY() is necessary
    TObjectPtr<class UResourceManager> ResourceManager;
    FName WeaponName;
    TObjectPtr<UDataTable> WeaponDataTable;
    FWeaponData* WeaponData;
    TObjectPtr<USkeletalMesh> WeaponMesh;
    TObjectPtr<UAnimInstance> CurrAnimInstance;
    TMap<FName, FAttackAnimationData> LoadedWeaponAnimations;

private:
    int32 AttackIndex;
    bool bIsRight;
    FName MontageName;
};

UCLASS()
class BLOODBORNE_API USawCleaver : public UWeaponInstance
{
    GENERATED_BODY()

public:
    //HardCoding?
    USawCleaver() : UWeaponInstance() {};
};

