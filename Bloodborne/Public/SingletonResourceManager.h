// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bloodborne.h"
#include "UObject/NoExportTypes.h"
#include "SingletonResourceManager.generated.h"

/**
 * 
 */
UCLASS()
class BLOODBORNE_API USingletonResourceManager : public UObject
{
	GENERATED_BODY()
	
private:
    static USingletonResourceManager* Instance;

    UPROPERTY()
    TObjectPtr<UDataTable> WeaponDataTable;
    UPROPERTY()
    TObjectPtr<UAnimInstance> AnimInstance;

protected:
    USingletonResourceManager() {}


public:
    static USingletonResourceManager* Get()
    {
        if (!Instance)
        {
            Instance = NewObject<USingletonResourceManager>();
            //Not to get deleted by GC
            Instance->AddToRoot();
        }
        return Instance;
    }

    void Initialize(UAnimInstance* InAnimInstance, UDataTable* InDataTable);
    const TObjectPtr<UDataTable> GetWeaponDataTable();
    const TObjectPtr<UAnimInstance> GetAnimInstance();
};

USingletonResourceManager* USingletonResourceManager::Instance = nullptr;
