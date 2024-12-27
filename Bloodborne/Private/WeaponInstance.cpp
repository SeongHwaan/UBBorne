// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponInstance.h"
#include "HunterAnimInstance.h"
#include "SingletonResourceManager.h"


UWeaponInstance::UWeaponInstance()
{
}

UWeaponInstance::UWeaponInstance(FName RowName)
{
    ResourceManager = USingletonResourceManager::Get();
    CurrAnimInstance = ResourceManager->GetAnimInstance();
    WeaponDataTable = ResourceManager->GetWeaponDataTable();
    if (WeaponDataTable)
    {
        WeaponData = ResourceManager->GetWeaponDataTable()->FindRow<FWeaponData>(RowName, TEXT("WeaponData"));
        LoadedWeaponAnimations = WeaponData->Animations;
        WeaponMesh = WeaponData->WeaponMesh;
    }
    else
        UE_LOG(LogTemp, Warning, TEXT("Instance DataTable error"));
}

const TObjectPtr<USkeletalMesh> UWeaponInstance::GetWeaponMesh()
{
    return WeaponMesh;
}

void USawCleaver::LightAttack()
{
    auto Anim = Cast<UHunterAnimInstance>(CurrAnimInstance);
    
    FName Key1 = FName(TEXT("123"));

    UAnimMontage* montage = *LoadedWeaponAnimations.Find(Key1);

    if (montage)
    {
        Anim->Montage_Play(montage, 1.0f);
    }
}
