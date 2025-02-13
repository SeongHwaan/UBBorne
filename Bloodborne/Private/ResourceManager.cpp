// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceManager.h"

UResourceManager::UResourceManager()
{
    static ConstructorHelpers::FObjectFinder<UDataTable> WeaponData(TEXT("/Game/Data/WeaponDataTable.WeaponDataTable"));
    if (WeaponData.Succeeded())
    {
        WeaponDataTable = WeaponData.Object;
    }
}

void UResourceManager::DoInitialize(UAnimInstance* InAnimInstance)
{
    PlayerAnimInstance = InAnimInstance;
}

TObjectPtr<UDataTable> UResourceManager::GetWeaponDataTable() const
{
    return WeaponDataTable;
}

TObjectPtr<UAnimInstance> UResourceManager::GetPlayerAnimInstance() const
{
    return PlayerAnimInstance;
}
