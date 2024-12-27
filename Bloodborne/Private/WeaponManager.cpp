// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponManager.h"
#include "Engine/DataTable.h"
#include "Animation/AnimMontage.h"
#include "WeaponData.h"
#include "WeaponInstance.h"
#include "SingletonResourceManager.h"

UWeaponManager::UWeaponManager()
{
    InitializeWeaponClasses();
}

TObjectPtr<class UWeaponInstance> UWeaponManager::SetRWeapon(FName WeaponName)
{
    auto WeaponClass = UWeaponManager::RightWeaponClassMap.Find(WeaponName);
    auto RWeapon = NewObject<UWeaponInstance>(this, *WeaponClass);
    return RWeapon;
}

TObjectPtr<class UWeaponInstance> UWeaponManager::SetLWeapon(FName WeaponName)
{
    auto WeaponClass = UWeaponManager::LeftWeaponClassMap.Find(WeaponName);
    auto LWeapon = NewObject<UWeaponInstance>(this, *WeaponClass);
    return LWeapon;
}

void UWeaponManager::InitializeWeaponClasses()
{
    RightWeaponClassMap.Add(TEXT("SawCleaver"), USawCleaver::StaticClass());
}
