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
    //auto Anim = Cast<UHunterAnimInstance>(CurrAnimInstance);
    //Anim->OnAttackEnd.AddUObject(this, &UWeaponInstance::ResetState);
}

const TObjectPtr<USkeletalMesh> UWeaponInstance::GetWeaponMesh()
{
    return WeaponMesh;
}

void UWeaponInstance::LightCombo(EWeaponForm Form)
{
    FName Key;
    if (Form == EWeaponForm::Regular)
    {
        
    }
        
    else if (Form == EWeaponForm::Transformed)    
    {


    }

    PlayAttackAnim(Key);
}

void UWeaponInstance::HeavyStart(EActionType Action, EWeaponForm Form)
{
    //CanDoNextAction을 애님 시작 전에 체크
    FName Key;
    if (Action == EActionType::HeavyAttack)
    {
        if (Form == EWeaponForm::Regular)
        {
            Key = FName(TEXT("RHeavyStart"));
        }
        else if (Form == EWeaponForm::Transformed)
        {

        }
    }

    PlayAttackAnim(Key);
}

void UWeaponInstance::HeavyEnd(EWeaponForm Form)
{
    FName Key;
    if (Form == EWeaponForm::Regular)
    {
        Key = FName(TEXT("RHeavyEnd"));
    }
    else if (Form == EWeaponForm::Transformed)
    {

    }

    PlayAttackAnim(Key);
}

void UWeaponInstance::ChargeEnd(EWeaponForm Form)
{
    FName Key;
    if (Form == EWeaponForm::Regular)
    {
        Key = FName(TEXT("RChargeEnd"));
    }
    else if (Form == EWeaponForm::Transformed)
    {

    }

    PlayAttackAnim(Key);
}


void UWeaponInstance::RollAttack(EActionType Action, EWeaponForm Form)
{
    FName Key;
    if (Action == EActionType::LightAttack)
    {
        if (Form == EWeaponForm::Regular)
        {
            Key = FName(TEXT("RRollLight"));
        }
        else if (Form == EWeaponForm::Transformed)
        {

        }
    }
    else if (Action == EActionType::HeavyAttack)
    {
        if (Form == EWeaponForm::Regular)
        {
            Key = FName(TEXT(""));
        }
        else if (Form == EWeaponForm::Transformed)
        {

        }
    }

    PlayAttackAnim(Key);
}

void UWeaponInstance::ResetState()
{
    AttackIndex = 1;
    UE_LOG(LogTemp, Warning, TEXT("Reset worked"));
}

void UWeaponInstance::PlayAttackAnim(FName Key)
{
    auto AnimationData = LoadedWeaponAnimations.Find(Key);
    if (AnimationData)
    {
        UAnimMontage* Montage = AnimationData->AttackMontage;
        if (Montage)
            CurrAnimInstance->Montage_Play(Montage, 1.0f);
    }
}
