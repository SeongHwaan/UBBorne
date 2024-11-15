// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bloodborne.h"
#include "GameFramework/Character.h"
#include "BBCharacter.generated.h"

UCLASS()
class BLOODBORNE_API ABBCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABBCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> Camera;

public:
	void Move(const FVector2D& Vector);
	void Look(const FVector2D& Vector);
	void CustomJump();
	void CustomStopJump();
	void Attack();

	bool GetHasMovementInput() const;
	float GetInputChangeRate() const;
	float GetInputIntensity() const;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool HasMovementInput = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float InputChangeRate = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float InputIntensity = 0.0f; 

private:
	float PreviousInputIntensity = 0.0f;
	float ChangeCheckInterval = 0.1f;
	float LastChangeCheckTime = 0.0f;

public:
	void LockOn();
	void LockOff();
	bool GetIsLockOn() const;
	bool IsTargetVisible(APawn* Pawn);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	bool IsLockOn = false;

	TObjectPtr<APawn> FindClosestPawn();

private:
	UPROPERTY(EditAnywhere, Category = "Camera")
	float MaxLockOnDistance = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	TObjectPtr<APawn> TargetPawn = nullptr;
};
