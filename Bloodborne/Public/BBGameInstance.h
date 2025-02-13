// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bloodborne.h"
#include "Engine/GameInstance.h"
#include "BBGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BLOODBORNE_API UBBGameInstance : public UGameInstance
{
	GENERATED_BODY()
	

public:
    UBBGameInstance();

    virtual void Init() override;

    void SetResourceManager(class AHunterCharacter* Character);

};
