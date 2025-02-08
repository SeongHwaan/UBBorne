// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponInstance.h"
#include "HunterAnimInstance.h"
#include "ResourceManager.h"

UWeaponInstance::UWeaponInstance()
{
    AttackIndex = 1;
    bIsRight = true;
    WeaponData = nullptr;
}

void UWeaponInstance::InitializeWeapon(FName RowName)
{
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
    }

    CurrAnimInstance = ResourceManager->GetAnimInstance();
    // Delegate in PostInitializeComponents or BeginPlay rather than a constructor.
    auto Anim = Cast<UHunterAnimInstance>(CurrAnimInstance);
    Anim->OnAttackEnd.AddUObject(this, &UWeaponInstance::ResetState);
}

const TObjectPtr<USkeletalMesh> UWeaponInstance::GetWeaponMesh()
{
    return WeaponMesh;
}

void UWeaponInstance::SetAttackIndex(int input)
{
    AttackIndex = input;
}

void UWeaponInstance::LightCombo(EWeaponForm Form)
{
    FName Key;
    if (Form == EWeaponForm::Regular)
    {
        switch (AttackIndex)
        {
        case 1:
            Key = FName(TEXT("RLightComboStart"));
            AttackIndex++;
            break;
        case 2:
            Key = FName(TEXT("RLightCombo1"));
            AttackIndex++;
            break;
        case 3:
            Key = FName(TEXT("RLightCombo2"));
            AttackIndex++;
            break;
        case 4:
            Key = FName(TEXT("RLightCombo3"));
            AttackIndex++;
            break;
        case 5:
            Key = FName(TEXT("RLightCombo4"));
            AttackIndex = 2;
            break;
        }
    }

    else if (Form == EWeaponForm::Transformed)
    {
    }

    PlayAttackAnim(Key);
}

void UWeaponInstance::HeavyStart(EWeaponForm Form)
{
    //CanDoNextAction을 애님 시작 전에 체크
    FName Key;

    if (Form == EWeaponForm::Regular)
    {
        Key = FName(TEXT("RHeavyStart"));
    }
    else if (Form == EWeaponForm::Transformed)
    {

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
