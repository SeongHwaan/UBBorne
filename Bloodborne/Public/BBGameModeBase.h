// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bloodborne.h"
#include "GameFramework/GameModeBase.h"
#include "BBGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class BLOODBORNE_API ABBGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	ABBGameModeBase();

	virtual void PostInitializeComponents() override;
	
};
