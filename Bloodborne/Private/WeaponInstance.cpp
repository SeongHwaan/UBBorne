// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponInstance.h"
#include "HunterAnimInstance.h"
#include "ResourceManager.h"

UWeaponInstance::UWeaponInstance(FName RowName)
{
    AttackIndex = 1;
    bIsRight = true;
    WeaponName = RowName;

    if (UWorld* World = GetWorld())
    {
        ResourceManager = World->GetGameInstance()->GetSubsystem<UResourceManager>();
        WeaponDataTable = ResourceManager->GetWeaponDataTable();
        if (WeaponDataTable)
        {
            WeaponData = WeaponDataTable->FindRow<FWeaponData>(WeaponName, TEXT("WeaponData"));
            LoadedWeaponAnimations = WeaponData->AnimationData;
            WeaponMesh = WeaponData->WeaponMesh;
        }
        else
            UE_LOG(LogTemp, Warning, TEXT("Instance DataTable error"));

        CurrAnimInstance = ResourceManager->GetAnimInstance();
    }
}

void UWeaponInstance::InitializeWeapon()
{
    // Delegate in PostInitializeComponents or BeginPlay rather than a constructor.
    auto Anim = Cast<UHunterAnimInstance>(CurrAnimInstance);
    Anim->OnAttackEnd.AddUObject(this, &UWeaponInstance::ResetState);
}

const TObjectPtr<USkeletalMesh> UWeaponInstance::GetWeaponMesh()
{
    return WeaponMesh;
}

void UWeaponInstance::RollAttack(EActionType Action, EWeaponForm Form)
{
    if (Action == EActionType::LightAttack)
    {
        if (Form == EWeaponForm::Regular)
        {
            FName Key1 = FName(TEXT("123"));
            auto AnimationData = *LoadedWeaponAnimations.Find(Key1);
            UAnimMontage* montage = AnimationData.AttackMontage;
            if (montage)
                CurrAnimInstance->Montage_Play(montage, 1.0f);
        }
        else if (Form == EWeaponForm::Transformed)
        {

        }
    }
}

void UWeaponInstance::ResetState()
{
    AttackIndex = 1;
    UE_LOG(LogTemp, Warning, TEXT("Reset worked"));
}

void USawCleaver::LightCombo()
{
    //FName Key1 = FName(TEXT("123"));

    //UAnimMontage* montage = *LoadedWeaponAnimations.Find(Key1);

    //if (montage)
    //{
    //    CurrAnimInstance->Montage_Play(montage, 1.0f);
    //}
}

void USawCleaver::HeavyCombo()
{
}

//void USawCleaver::RollAttack(EActionType Action, EWeaponForm Form)
//{
//}
