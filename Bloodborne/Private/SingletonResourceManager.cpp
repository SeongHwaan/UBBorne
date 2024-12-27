// Fill out your copyright notice in the Description page of Project Settings.


#include "SingletonResourceManager.h"

void USingletonResourceManager::Initialize(UAnimInstance* InAnimInstance, UDataTable* InDataTable)
{
    if (!InAnimInstance || !InDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("USingletonResourceManager: Invalid parameters for Initialize."));
        return;
    }
    AnimInstance = InAnimInstance;
    WeaponDataTable = InDataTable;
}

const TObjectPtr<UDataTable> USingletonResourceManager::GetWeaponDataTable()
{
    return WeaponDataTable;
}

const TObjectPtr<UAnimInstance> USingletonResourceManager::GetAnimInstance()
{
    return AnimInstance;
}
