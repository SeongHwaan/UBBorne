// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bloodborne.h"
#include "Engine/DataTable.h"
#include "AttackAnimationData.h"
#include "WeaponData.generated.h"

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName WeaponName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<USkeletalMesh> WeaponMesh;


    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 DefaultDamage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Durability;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, FAttackAnimationData> AnimationData;
};