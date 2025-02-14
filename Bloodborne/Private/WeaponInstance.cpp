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

void UWeaponInstance::SetWeaponMeshComp(USkeletalMeshComponent* Component)
{
    WeaponMeshComp = Component;
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
            break;
        case 1:
            MontageName = FName(TEXT("RLightCombo1"));
            break;
        case 2:
            MontageName = FName(TEXT("RLightCombo2"));
            break;
        case 3:
            MontageName = FName(TEXT("RLightCombo3"));
            break;
        case 4:
            MontageName = FName(TEXT("RLightCombo4"));
            AttackIndex = 0;
            break;
        }
    }
    else if (Form == EWeaponForm::Transformed)
    {
        switch (AttackIndex)
        {
        case 0:
            MontageName = FName(TEXT("TLightComboStart"));
            break;
        case 1:
            MontageName = FName(TEXT("TLightCombo1"));
            break;
        case 2:
            MontageName = FName(TEXT("TLightCombo2"));
            AttackIndex = 0;
            break;
        }
    }

    SetAnimData(MontageName);
    AttackIndex++;
    PlayAttackAnim();
}

void UWeaponInstance::PlayHeavyStart(EWeaponForm Form)
{
    CheckLeftRight();

    //CanDoNextAction을 애님 시작 전에 체크
    if (Form == EWeaponForm::Regular)
    {
        MontageName = FName(TEXT("RHeavyStart"));
    }
    else if (Form == EWeaponForm::Transformed)
    {
        switch (AttackIndex)
        {
        case 0:
            MontageName = FName(TEXT("THeavyStart0"));
            break;
        case 1:
            MontageName = FName(TEXT("THeavyStart1"));
            break;
        }
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
        switch (AttackIndex)
        {
        case 0:
            MontageName = FName(TEXT("THeavyEnd0"));
            break;
        case 1:
            MontageName = FName(TEXT("THeavyEnd1"));
            break;
        }
    }
    SetAnimData(MontageName);
    NextLeftRight();
    PlayAttackAnim();
}

void UWeaponInstance::PlayChargeEnd(EWeaponForm Form)
{
    if (Form == EWeaponForm::Regular)
    {
        MontageName = FName(TEXT("RChargeEnd"));
    }
    else if (Form == EWeaponForm::Transformed)
    {
        switch (AttackIndex)
        {
        case 0:
            MontageName = FName(TEXT("TChargeEnd0"));
            break;
        case 1:
            MontageName = FName(TEXT("TChargeEnd1"));
            break;
        }
    }
    SetAnimData(MontageName);
    NextLeftRight();
    PlayAttackAnim();
}

void UWeaponInstance::PlayHeavyAfterCharge()
{
    //코드가 좀...
    //ChargeEnd가 다음 방향을 바꿔버려서 다시 바꿔줘야 함
    AttackIndex = (AttackIndex + 1) % 2;
    switch (AttackIndex)
    {
    case 0:
        MontageName = FName(TEXT("THeavyAfterCharge0"));
        break;
    case 1:
        MontageName = FName(TEXT("THeavyAfterCharge1"));
        break;
    }
    SetAnimData(MontageName);
    NextLeftRight();
    PlayAttackAnim();
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
            MontageName = FName(TEXT("TRollLight"));
        }
    }
    else if (Action == EActionType::HeavyAttack)
    {
        PlayHeavyStart(Form);
        return;
    }
    else if (Action == EActionType::FormChange)
    {
        PlayFormChange(Form, true);
        return;
    }
    SetAnimData(MontageName);
    NextLeftRight();
    PlayAttackAnim();
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
            MontageName = FName(TEXT("TBackstepLight"));
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
            MontageName = FName(TEXT("TBackstepHeavy"));
        }
    }
    else if (Action == EActionType::FormChange)
    {
        PlayFormChange(Form, true);
        return;
    }
    SetAnimData(MontageName);
    NextLeftRight();
    PlayAttackAnim();
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
            if (FMath::Abs(angle) < 67.5)
                MontageName = FName(TEXT("TRollLight"));
            else if (-112.5 <= angle && angle < -67.5)
                MontageName = FName(TEXT("TDodgeLeft"));
            else
                MontageName = FName(TEXT("TDodgeDefault"));
        }
    }
    else if (Action == EActionType::HeavyAttack)
    {
        PlayHeavyStart(Form);
        return;
    }
    else if (Action == EActionType::FormChange)
    {
        PlayFormChange(Form, true);
        return;
    }
    SetAnimData(MontageName);
    NextLeftRight();
    PlayAttackAnim();
}


void UWeaponInstance::PlayJumpAttack(EWeaponForm Form)
{
    if (Form == EWeaponForm::Regular)
    {
        MontageName = FName(TEXT("RLeapHeavy"));
    }
    else if (Form == EWeaponForm::Transformed)
    {
        MontageName = FName(TEXT("TLeapHeavy"));
    }
    //공격 교체?
    SetAnimData(MontageName);
    NextLeftRight();
    PlayAttackAnim();
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
            MontageName = FName(TEXT("TSprintLight"));
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
            MontageName = FName(TEXT("TSprintHeavy"));
        }
    }
    //Sprint에서는 일반 교체
    else if (Action == EActionType::FormChange)
    {
        PlayFormChange(Form, false);
        return;
    }
    SetAnimData(MontageName);
    NextLeftRight();
    PlayAttackAnim();
}

void UWeaponInstance::PlayFormChange(EWeaponForm Form, bool bIsAttacking)
{
    CheckLeftRight();

    if (bIsAttacking)
    {
        if (Form == EWeaponForm::Regular)
        {
            switch (AttackIndex)
            {
            case 0:
                MontageName = FName(TEXT("RTransAttackToL"));
                break;
            case 1:
                MontageName = FName(TEXT("RTransAttackToR"));
                break;
            }
        }
        else if (Form == EWeaponForm::Transformed)
        {
            switch (AttackIndex)
            {
            case 0:
                MontageName = FName(TEXT("TTransAttackToL"));
                break;
            case 1:
                MontageName = FName(TEXT("TTransAttackToR"));
                break;
            }
        }
    }
    else
    {
        if (Form == EWeaponForm::Regular)
        {
            MontageName = FName(TEXT("RToT"));
        }
        else if (Form == EWeaponForm::Transformed)
        {
            MontageName = FName(TEXT("TToR"));
        }
    }
    SetAnimData(MontageName);
    NextLeftRight();
    PlayAttackAnim();
}

void UWeaponInstance::CheckLeftRight()
{
    AttackIndex = AttackIndex % 2;
}

void UWeaponInstance::NextLeftRight()
{
    //이런 설계면 굳이 CurrentPos를 왜...
    int CurrentPos = static_cast<int>(CurrAnimData->StartPos);
    AttackIndex = (CurrentPos + 1) % 2;
}

void UWeaponInstance::ResetState()
{
    AttackIndex = 0;
    UE_LOG(LogTemp, Warning, TEXT("Reset worked"));
}

void UWeaponInstance::SetAnimData(FName Key)
{
    CurrAnimData = LoadedWeaponAnimations.Find(Key);
}

void UWeaponInstance::PlayAttackAnim()
{
    if (CurrAnimData)
    {
        auto Montage = CurrAnimData->AttackMontage;
        if (Montage)
        {
            //Montage_Play가 끝나야 PlayAttackAnim() 함수가 끝나나?
            PlayerAnimInstance->Montage_Play(Montage, 1.0f);
        }
    }
}
