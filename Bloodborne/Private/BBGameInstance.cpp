// Fill out your copyright notice in the Description page of Project Settings.


#include "BBGameInstance.h"
#include "ResourceManager.h"
#include "HunterCharacter.h"

UBBGameInstance::UBBGameInstance()
{
}

void UBBGameInstance::Init()
{

    Super::Init();

    //GameInstance의 Subsystem은 Init()에서 초기화됨.
    ResourceManager = GetSubsystem<UResourceManager>();
}

void UBBGameInstance::SetResourceManager(AHunterCharacter* Character)
{
    if (Character)
    {
        auto AnimInstance = Character->GetMesh()->GetAnimInstance();
        auto WeaponDataTable = Character->GetWeaponDataTable();
        ResourceManager->DoInitialize(AnimInstance, WeaponDataTable);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GameInstance: ResourceManager is nullptr!"));
    }
}

UResourceManager* UBBGameInstance::GetResourceManager() const
{
    return ResourceManager;
}