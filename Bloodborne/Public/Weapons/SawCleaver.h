// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bloodborne.h"
#include "WeaponInstance.h"
#include "SawCleaver.generated.h"

/**
 * 
 */
UCLASS()
class BLOODBORNE_API USawCleaver : public UWeaponInstance
{
	GENERATED_BODY()
	
public:

    USawCleaver();

    void PlayFormChange(EWeaponForm Form, bool bIsAttacking, class USkeletalMeshComponent* WeaponAnim);

private:

    UAnimSequence* SawCleaverRToT = nullptr;
    UAnimSequence* SawCleaverTToR = nullptr;
};