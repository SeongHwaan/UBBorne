// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bloodborne.h"
#include "Engine/DataTable.h"
#include "AttackAnimationData.generated.h"

UENUM(BlueprintType)
enum class EStartPos : uint8
{
    Right = 0 UMETA(DisplayName = "Right"),
    Left = 1 UMETA(DisplayName = "Left"),
    None = 2 UMETA(DisplayName = "None")
};


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

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EStartPos StartPos;
};
