// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bloodborne.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ResourceManager.generated.h"

/**
 * 
 */
UCLASS()
class BLOODBORNE_API UResourceManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

    UResourceManager();

    void DoInitialize(UAnimInstance* InAnimInstance);
    TObjectPtr<UDataTable> GetWeaponDataTable() const;
    TObjectPtr<UAnimInstance> GetPlayerAnimInstance() const;

private:

    TObjectPtr<UDataTable> WeaponDataTable;
    TObjectPtr<UAnimInstance> PlayerAnimInstance;
};
