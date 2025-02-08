// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bloodborne.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "BBPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLOODBORNE_API ABBPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	ABBPlayerController();

	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* aPawn) override;	//Possess is deprecated

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	void Move(const FInputActionValue& Value);
    void MoveEnd();
	void Look(const FInputActionValue& Value);
	void StartJump();
	void StopJump();

    void LockOn();
	void Dodge();
    void DodgeEnd();
    void StartSprint();
	void Sprinting();
	void StopSprint();

    void LightAttack();
    void HeavyAttackStart();
    void HeavyAttackEnd();
    void WeaponChange();

private:

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	TObjectPtr<class UInputMappingContext> DefaultContext;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	TObjectPtr<class UInputAction> LockOnAction;
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	TObjectPtr<class UInputAction> DodgeAction;
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	TObjectPtr<class UInputAction> SprintAction;

    UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
    TObjectPtr<class UInputAction> LightAttackAction;

    UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
    TObjectPtr<class UInputAction> HeavyAttackAction;

    UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
    TObjectPtr<class UInputAction> WeaponChangeAction;

    //Can be improved to preserve polymorphism
	UPROPERTY()
	TObjectPtr<class AHunterCharacter> BBCharacter;

    class UEnhancedInputComponent* EnhancedInputComponent;
};
