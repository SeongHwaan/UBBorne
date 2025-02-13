// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponInstance.h"
#include "HunterAnimInstance.h"
#include "ResourceManager.h"
#include "BBGameInstance.h"

UWeaponInstance::UWeaponInstance()
{
    AttackIndex = 0;
    bIsRight = true;
    WeaponData = nullptr;
}

void UWeaponInstance::InitializeWeapon(FName RowName)
{
    WeaponName = RowName;
    if (UWorld* World = GetWorld())
    {
        auto MyGameInstance = Cast<UBBGameInstance>(World->GetGameInstance());
        auto ResourceManager = MyGameInstance->GetSubsystem<UResourceManager>();
        auto WeaponDataTable = ResourceManager->GetWeaponDataTable();
        PlayerAnimInstance = ResourceManager->GetPlayerAnimInstance();
        if (WeaponDataTable)
        {
            WeaponData = WeaponDataTable->FindRow<FWeaponData>(WeaponName, TEXT("WeaponData"));
            LoadedWeaponAnimations = WeaponData->AnimationData;
            WeaponMesh = WeaponData->WeaponMesh;
        }
    }

    // Delegate in PostInitializeComponents or BeginPlay rather than a constructor.
    if (PlayerAnimInstance)
    {
        auto Anim = Cast<UHunterAnimInstance>(PlayerAnimInstance);
        Anim->OnAttackEnd.AddUObject(this, &UWeaponInstance::ResetState);
    }
    else
        UE_LOG(LogTemp, Warning, TEXT("no AnimInstance"));
}

TObjectPtr<USkeletalMesh> UWeaponInstance::GetWeaponMesh() const
{
    return WeaponMesh;
}

void UWeaponInstance::SetAttackIndex(int input)
{
    AttackIndex = input;
}


void UWeaponInstance::PlayLightCombo(EWeaponForm Form)
{
    if (Form == EWeaponForm::Regular)
    {
        switch (AttackIndex)
        {
        case 0:
            MontageName = FName(TEXT("RLightComboStart"));
            AttackIndex++;
            break;
        case 1:
            MontageName = FName(TEXT("RLightCombo1"));
            AttackIndex++;
            break;
        case 2:
            MontageName = FName(TEXT("RLightCombo2"));
            AttackIndex++;
            break;
        case 3:
            MontageName = FName(TEXT("RLightCombo3"));
            AttackIndex++;
            break;
        case 4:
            MontageName = FName(TEXT("RLightCombo4"));
            AttackIndex = 1;
            break;
        }
    }

    else if (Form == EWeaponForm::Transformed)
    {
    }

    SetAnimData(MontageName);
    PlayAttackAnim();
}

void UWeaponInstance::PlayHeavyStart(EWeaponForm Form)
{
    //CanDoNextAction을 애님 시작 전에 체크
    if (Form == EWeaponForm::Regular)
    {
        MontageName = FName(TEXT("RHeavyStart"));
    }
    else if (Form == EWeaponForm::Transformed)
    {

    }
    SetAnimData(MontageName);
    PlayAttackAnim();
}

void UWeaponInstance::PlayHeavyEnd(EWeaponForm Form)
{
    if (Form == EWeaponForm::Regular)
    {
        MontageName = FName(TEXT("RHeavyEnd"));
    }
    else if (Form == EWeaponForm::Transformed)
    {

    }
    SetAnimData(MontageName);
    PlayAttackAnim();
    CheckLeftRight();
}

void UWeaponInstance::PlayChargeEnd(EWeaponForm Form)
{
    if (Form == EWeaponForm::Regular)
    {
        MontageName = FName(TEXT("RChargeEnd"));
    }
    else if (Form == EWeaponForm::Transformed)
    {

    }
    SetAnimData(MontageName);
    PlayAttackAnim();
    CheckLeftRight();
}


void UWeaponInstance::PlayRollAttack(EActionType Action, EWeaponForm Form)
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
            MontageName = FName(TEXT("RHeavyStart"));
        }
        else if (Form == EWeaponForm::Transformed)
        {

        }
    }
    SetAnimData(MontageName);
    PlayAttackAnim();
    CheckLeftRight();
}

void UWeaponInstance::PlayBackstepAttack(EActionType Action, EWeaponForm Form)
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
    SetAnimData(MontageName);
    PlayAttackAnim();
    CheckLeftRight();
}

void UWeaponInstance::PlayDodgeAttack(EActionType Action, EWeaponForm Form, float angle)
{
    if (Action == EActionType::LightAttack)
    {
        if (Form == EWeaponForm::Regular)
        {
            if (FMath::Abs(angle) < 67.5)
                MontageName = FName(TEXT("RRollLight"));
            else if ( 67.5 <= angle && angle < 112.5)
                MontageName = FName(TEXT("RDodgeRight"));
            else
                MontageName = FName(TEXT("RDodgeDefault"));
        }
        else if (Form == EWeaponForm::Transformed)
        {

        }
    }
    else if (Action == EActionType::HeavyAttack)
    {
        if (Form == EWeaponForm::Regular)
        {
            MontageName = FName(TEXT("RHeavyStart"));
        }
        else if (Form == EWeaponForm::Transformed)
        {

        }
    }
    SetAnimData(MontageName);
    PlayAttackAnim();
    CheckLeftRight();
}


void UWeaponInstance::PlayJumpAttack(EWeaponForm Form)
{
    if (Form == EWeaponForm::Regular)
    {
        MontageName = FName(TEXT("RLeapHeavy"));
    }
    else if (Form == EWeaponForm::Transformed)
    {

    }
    SetAnimData(MontageName);
    PlayAttackAnim();
    CheckLeftRight();
}

void UWeaponInstance::PlaySprintAttack(EActionType Action, EWeaponForm Form)
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
    SetAnimData(MontageName);
    PlayAttackAnim();
    CheckLeftRight();
}

void UWeaponInstance::PlayFormChange(EWeaponForm Form, bool bIsAttacking, USkeletalMeshComponent* WeaponMeshComp)
{
    if (bIsAttacking)
    {
        if (Form == EWeaponForm::Regular)
        {
            MontageName = FName(TEXT("RTransAttackToR"));
        }
    }
    else
    {
        MontageName = FName(TEXT("RToT"));
    }
    SetAnimData(MontageName);
    PlayAttackAnim();
    CheckLeftRight();
}

int UWeaponInstance::CheckLeftRight()
{
    int CurrentPos = static_cast<int>(AnimData->StartPos);
    AttackIndex = (CurrentPos + 1) % 2;
    return AttackIndex;
}

void UWeaponInstance::ResetState()
{
    AttackIndex = 0;
    UE_LOG(LogTemp, Warning, TEXT("Reset worked"));
}

void UWeaponInstance::SetAnimData(FName Key)
{
    AnimData = LoadedWeaponAnimations.Find(Key);
}

void UWeaponInstance::PlayAttackAnim()
{
    if (AnimData)
    {
        auto Montage = AnimData->AttackMontage;
        if (Montage)
        {
            PlayerAnimInstance->Montage_Play(Montage, 1.0f);
        }
    }
}
