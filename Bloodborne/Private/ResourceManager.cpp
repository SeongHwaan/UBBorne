// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceManager.h"

void UResourceManager::Initialize(UAnimInstance* InAnimInstance, UDataTable* InDataTable)
{
    AnimInstance = InAnimInstance;
    WeaponDataTable = InDataTable;
}

const TObjectPtr<UDataTable> UResourceManager::GetWeaponDataTable()
{
    return WeaponDataTable;
}

const TObjectPtr<UAnimInstance> UResourceManager::GetAnimInstance()
{
    return AnimInstance;
}
