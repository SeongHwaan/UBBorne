﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponManager.h"
#include "Engine/DataTable.h"
#include "Animation/AnimMontage.h"
#include "WeaponData.h"
#include "WeaponInstance.h"
#include "Weapons/SawCleaver.h"

UWeaponManager::UWeaponManager()
{
    InitializeWeaponClasses();
}

TObjectPtr<UWeaponInstance> UWeaponManager::LoadRWeapon(FName WeaponName)
{
    //Make Exception Later
    auto WeaponClass = UWeaponManager::RightWeaponClassMap.Find(WeaponName);
    if (!WeaponClass || !*WeaponClass)
    {
        return nullptr; 
    }
    if (RWeapon)
    {
        RWeapon->ConditionalBeginDestroy();  
        RWeapon = nullptr;
    }
    RWeapon = NewObject<UWeaponInstance>(this, *WeaponClass);
    RWeapon->InitializeWeapon(WeaponName);
    return RWeapon;
}

TObjectPtr<UWeaponInstance> UWeaponManager::LoadLWeapon(FName WeaponName)
{
    auto WeaponClass = UWeaponManager::LeftWeaponClassMap.Find(WeaponName);
    if (!WeaponClass || !*WeaponClass)
    {
        return nullptr; 
    }
    if (LWeapon)
    {
        LWeapon->ConditionalBeginDestroy();
        LWeapon = nullptr;
    }
    LWeapon = NewObject<UWeaponInstance>(this, *WeaponClass);
    LWeapon->InitializeWeapon(WeaponName);
    return LWeapon;
}

void UWeaponManager::InitializeWeaponClasses()
{
    RightWeaponClassMap.Add(TEXT("SawCleaver"), USawCleaver::StaticClass());
}
