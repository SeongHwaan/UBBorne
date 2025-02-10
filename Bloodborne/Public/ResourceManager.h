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

    UResourceManager() {};

    void DoInitialize(UAnimInstance* InAnimInstance, UDataTable* InDataTable);
    TObjectPtr<UDataTable> GetWeaponDataTable() const;
    TObjectPtr<UAnimInstance> GetAnimInstance() const;

private:

    TObjectPtr<UDataTable> WeaponDataTable;
    TObjectPtr<UAnimInstance> AnimInstance;
	
};
