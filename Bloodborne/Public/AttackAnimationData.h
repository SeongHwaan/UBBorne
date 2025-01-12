// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bloodborne.h"
#include "Engine/DataTable.h"
#include "AttackAnimationData.generated.h"


USTRUCT(BlueprintType)
struct FAttackAnimationData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UAnimMontage* AttackMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DamageRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StaminaCost;
};
