// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bloodborne.h"
#include "UObject/NoExportTypes.h"
#include "WeaponManager.generated.h"

/**
 * 
 */
UCLASS()
class BLOODBORNE_API UWeaponManager : public UObject
{
	GENERATED_BODY()
	
public:

    UWeaponManager();

    void InitializeWeaponClasses();

    // Make it after Inventory
    // void SwitchWeapon():

    TObjectPtr<class UWeaponInstance> SetRWeapon(FName WeaponName);
    TObjectPtr<class UWeaponInstance> SetLWeapon(FName WeaponName);

private:
    
    TMap<FName, TSubclassOf<UWeaponInstance>> RightWeaponClassMap;
    TMap<FName, TSubclassOf<UWeaponInstance>> LeftWeaponClassMap;
};
