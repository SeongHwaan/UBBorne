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
}

void UBBGameInstance::SetResourceManager(AHunterCharacter* Character)
{
    if (Character)
    {
        auto AnimInstance = Character->GetMesh()->GetAnimInstance();
        GetSubsystem<UResourceManager>()->DoInitialize(AnimInstance);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GameInstance: ResourceManager is nullptr!"));
    }
}