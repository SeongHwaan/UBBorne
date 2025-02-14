// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/SawCleaver.h"

USawCleaver::USawCleaver()
{
    static ConstructorHelpers::FObjectFinder<UAnimSequence> AnimRToT(TEXT("/Game/Bloodborne/Animations/Weapon/SawCleaver/WeaponChange/SawCleaverRToT.SawCleaverRToT"));
    if (AnimRToT.Succeeded())
    {
        SawCleaverRToT = AnimRToT.Object;
    }

    static ConstructorHelpers::FObjectFinder<UAnimSequence> AnimTToR(TEXT("/Game/Bloodborne/Animations/Weapon/SawCleaver/WeaponChange/SawCleaverTToR.SawCleaverTToR"));
    if (AnimTToR.Succeeded())
    {
        SawCleaverTToR = AnimTToR.Object;
    }
}

void USawCleaver::PlayFormChange(EWeaponForm Form, bool bIsAttacking)
{
    Super::PlayFormChange(Form, bIsAttacking);

    if (Form == EWeaponForm::Regular)
        WeaponMeshComp->PlayAnimation(SawCleaverRToT, false);
    else
        WeaponMeshComp->PlayAnimation(SawCleaverTToR, false);
}
