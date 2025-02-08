// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceManager.h"

void UResourceManager::DoInitialize(UAnimInstance* InAnimInstance, UDataTable* InDataTable)
{
    AnimInstance = InAnimInstance;
    WeaponDataTable = InDataTable;
}

TObjectPtr<UDataTable> UResourceManager::GetWeaponDataTable() const
{
    return WeaponDataTable;
}

TObjectPtr<UAnimInstance> UResourceManager::GetAnimInstance() const
{
    return AnimInstance;
}
