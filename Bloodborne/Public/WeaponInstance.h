// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bloodborne.h"
#include "WeaponData.h"
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
    UWeaponInstance(FName Name);

    const TObjectPtr<USkeletalMesh> GetWeaponMesh();

    virtual void LightAttack() {};

protected:
    // Don't think UPROPERTY() is necessary
    TObjectPtr<class USingletonResourceManager> ResourceManager;
    TObjectPtr<UDataTable> WeaponDataTable;
    FWeaponData* WeaponData;
    TObjectPtr<USkeletalMesh> WeaponMesh;
    TObjectPtr<UAnimInstance> CurrAnimInstance;
    TMap<FName, UAnimMontage*> LoadedWeaponAnimations;
};

UCLASS()
class BLOODBORNE_API USawCleaver : public UWeaponInstance
{
    GENERATED_BODY()

public:
    //HardCoding?
    USawCleaver() : UWeaponInstance(FName(TEXT("SawCleaver"))) {};

    void LightAttack() override;

};

