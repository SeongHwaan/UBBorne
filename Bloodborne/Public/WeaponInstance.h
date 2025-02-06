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
    UWeaponInstance() {};
    UWeaponInstance(FName Name);

    void InitializeWeapon();

    const TObjectPtr<USkeletalMesh> GetWeaponMesh();

    virtual void LightCombo(EWeaponForm Form);
    virtual void HeavyStart(EActionType Action, EWeaponForm Form);
    virtual void HeavyEnd(EWeaponForm Form);
    virtual void ChargeEnd(EWeaponForm Form);
    virtual void RollAttack(EActionType Action, EWeaponForm Form);
    virtual void BackstepAttack() {};
    virtual void DodgeAttack() {};
    virtual void JumpAttack() {};
    virtual void SprintAttack() {};
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
};

UCLASS()
class BLOODBORNE_API USawCleaver : public UWeaponInstance
{
    GENERATED_BODY()

public:
    //HardCoding?
    USawCleaver() : UWeaponInstance(FName(TEXT("SawCleaver"))) {};
};

