﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bloodborne.h"
#include "Engine/DataTable.h"
#include "WeaponData.generated.h"

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName WeaponName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class UWeaponInstance> WeaponClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, class UAnimMontage*> Animations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<USkeletalMesh> WeaponMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 AttackPower;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Durability;
};