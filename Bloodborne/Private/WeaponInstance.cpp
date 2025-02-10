// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponInstance.h"
#include "HunterAnimInstance.h"
#include "ResourceManager.h"
#include "BBGameInstance.h"

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
        UBBGameInstance* MyGameInstance = Cast<UBBGameInstance>(World->GetGameInstance());
        ResourceManager = MyGameInstance->GetResourceManager();
        WeaponDataTable = ResourceManager->GetWeaponDataTable();
        if (WeaponDataTable)
        {
            WeaponData = WeaponDataTable->FindRow<FWeaponData>(WeaponName, TEXT("WeaponData"));
            LoadedWeaponAnimations = WeaponData->AnimationData;
            WeaponMesh = WeaponData->WeaponMesh;
        }
        CurrAnimInstance = ResourceManager->GetAnimInstance();
    }

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
    if (Form == EWeaponForm::Regular)
    {
        switch (AttackIndex)
        {
        case 1:
            MontageName = FName(TEXT("RLightComboStart"));
            AttackIndex++;
            break;
        case 2:
            MontageName = FName(TEXT("RLightCombo1"));
            AttackIndex++;
            break;
        case 3:
            MontageName = FName(TEXT("RLightCombo2"));
            AttackIndex++;
            break;
        case 4:
            MontageName = FName(TEXT("RLightCombo3"));
            AttackIndex++;
            break;
        case 5:
            MontageName = FName(TEXT("RLightCombo4"));
            AttackIndex = 2;
            break;
        }
    }

    else if (Form == EWeaponForm::Transformed)
    {
    }

    PlayAttackAnim(MontageName);
}

void UWeaponInstance::HeavyStart(EWeaponForm Form)
{
    //CanDoNextAction을 애님 시작 전에 체크
    if (Form == EWeaponForm::Regular)
    {
        MontageName = FName(TEXT("RHeavyStart"));
    }
    else if (Form == EWeaponForm::Transformed)
    {

    }


    PlayAttackAnim(MontageName);
}

void UWeaponInstance::HeavyEnd(EWeaponForm Form)
{
    if (Form == EWeaponForm::Regular)
    {
        MontageName = FName(TEXT("RHeavyEnd"));
    }
    else if (Form == EWeaponForm::Transformed)
    {

    }

    PlayAttackAnim(MontageName);
}

void UWeaponInstance::ChargeEnd(EWeaponForm Form)
{
    if (Form == EWeaponForm::Regular)
    {
        MontageName = FName(TEXT("RChargeEnd"));
    }
    else if (Form == EWeaponForm::Transformed)
    {

    }

    PlayAttackAnim(MontageName);
}


void UWeaponInstance::RollAttack(EActionType Action, EWeaponForm Form)
{
    if (Action == EActionType::LightAttack)
    {
        if (Form == EWeaponForm::Regular)
        {
            MontageName = FName(TEXT("RRollLight"));
        }
        else if (Form == EWeaponForm::Transformed)
        {

        }
    }
    else if (Action == EActionType::HeavyAttack)
    {
        if (Form == EWeaponForm::Regular)
        {
            MontageName = FName(TEXT(""));
        }
        else if (Form == EWeaponForm::Transformed)
        {

        }
    }

    PlayAttackAnim(MontageName);
}

void UWeaponInstance::BackstepAttack(EActionType Action, EWeaponForm Form)
{
    if (Action == EActionType::LightAttack)
    {
        if (Form == EWeaponForm::Regular)
        {
            MontageName = FName(TEXT("RBackstepLight"));
        }
        else if (Form == EWeaponForm::Transformed)
        {

        }
    }
    else if (Action == EActionType::HeavyAttack)
    {
        if (Form == EWeaponForm::Regular)
        {
            MontageName = FName(TEXT("RBackstepHeavy"));
        }
        else if (Form == EWeaponForm::Transformed)
        {

        }
    }
}


void UWeaponInstance::JumpAttack(EWeaponForm Form)
{
    if (Form == EWeaponForm::Regular)
    {
        MontageName = FName(TEXT("RLeapHeavy"));
    }
    else if (Form == EWeaponForm::Transformed)
    {

    }
}

void UWeaponInstance::SprintAttack(EActionType Action, EWeaponForm Form)
{
    if (Action == EActionType::LightAttack)
    {
        if (Form == EWeaponForm::Regular)
        {
            MontageName = FName(TEXT("RSprintLight"));
        }
        else if (Form == EWeaponForm::Transformed)
        {

        }
    }
    else if (Action == EActionType::HeavyAttack)
    {
        if (Form == EWeaponForm::Regular)
        {
            MontageName = FName(TEXT("RSprintHeavy"));
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

void UWeaponInstance::PlayAttackAnim(FName Key)
{
    auto AnimationData = LoadedWeaponAnimations.Find(Key);
    if (AnimationData)
    {
        UAnimMontage* Montage = AnimationData->AttackMontage;
        if (Montage)
        {
            CurrAnimInstance->Montage_Play(Montage, 1.0f);
        }
    }
}
