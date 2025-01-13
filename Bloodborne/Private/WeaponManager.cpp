// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponManager.h"
#include "Engine/DataTable.h"
#include "Animation/AnimMontage.h"
#include "WeaponData.h"
#include "WeaponInstance.h"

UWeaponManager::UWeaponManager()
{
    InitializeWeaponClasses();
}

TObjectPtr<class UWeaponInstance> UWeaponManager::LoadRWeapon(FName WeaponName)
{
    //Make Exception Later
    auto WeaponClass = UWeaponManager::RightWeaponClassMap.Find(WeaponName);
    auto RWeapon = NewObject<UWeaponInstance>(this, *WeaponClass);
    RWeapon->InitializeWeapon();
    return RWeapon;
}

TObjectPtr<class UWeaponInstance> UWeaponManager::LoadLWeapon(FName WeaponName)
{
    auto WeaponClass = UWeaponManager::LeftWeaponClassMap.Find(WeaponName);
    auto LWeapon = NewObject<UWeaponInstance>(this, *WeaponClass);
    return LWeapon;
}

void UWeaponManager::InitializeWeaponClasses()
{
    RightWeaponClassMap.Add(TEXT("SawCleaver"), USawCleaver::StaticClass());
}
