// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bloodborne.h"
#include "BBCharacter.h"
#include "HunterCharacter.generated.h"

UENUM(BlueprintType)
enum class EMovementState : uint8
{
    None UMETA(DisplayName = "None"), 
    Walk UMETA(DisplayName = "Walk"),
    Run UMETA(DisplayName = "Run"),
    Sprint UMETA(DisplayName = "Sprint"),
};

UCLASS()
class BLOODBORNE_API AHunterCharacter : public ABBCharacter
{
	GENERATED_BODY()
	
public:

    AHunterCharacter();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
    virtual void PostInitializeComponents() override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class USpringArmComponent> SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class UCameraComponent> Camera;

    UPROPERTY()
    TObjectPtr<class UHunterAnimInstance> HAnim;

//Normal
public:
    void Move(const FVector2D& Vector);
    void Look(const FVector2D& Vector);
    void CustomJump();
    void CustomStopJump();
    void Attack();
    void Sprinting();
    void StopSprinting();
    void Dodging();
    void StopDodging();

    bool GetHasMovementInput() const;
    float GetInputChangeRate() const;
    float GetInputIntensity() const;
    bool GetIsSprinting() const;
    bool GetIsDodging() const;

    float GetDirectionAngle() const;
    EMovementState GetMovementState() const;

    void SetMovementState(EMovementState State);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool HasMovementInput = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float InputChangeRate = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float InputIntensity = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool IsSprinting = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool IsDodging = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool CanAction = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MovementDirectionAngle = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float targetSpeed = 0.0f;

private:
    float PreviousInputIntensity = 0.0f;
    float ChangeCheckInterval = 0.1f;
    float LastChangeCheckTime = 0.0f;

//LockOn
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

//MovementState
private:

    class MovementState
    {
    public:
        virtual void Move(AHunterCharacter* Chr) = 0;
        virtual ~MovementState() = default;
    };

    class NoneState : public MovementState
    {
        void Move(AHunterCharacter* Chr) override;
    };

    class WalkState : public MovementState
    {
        void Move(AHunterCharacter* Chr) override;
    };

    class RunState : public MovementState
    {
        void Move(AHunterCharacter* Chr) override;
    };

    class SprintState : public MovementState
    {
        void Move(AHunterCharacter* Chr) override;
    };

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    EMovementState ECurrentMovementState;

    MovementState* CurrentMovementState;

    std::unique_ptr<NoneState> noneState = std::make_unique<NoneState>(); 
    std::unique_ptr<WalkState> walkState = std::make_unique<WalkState>();
    std::unique_ptr<RunState> runState = std::make_unique<RunState>();
    std::unique_ptr<SprintState> sprintState = std::make_unique<SprintState>();

//Weapon
public:

    void SetRightWeapon(class ABBWeapon* NewWeapon);

    FName RightWeaponSocket;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    TSubclassOf<class ABBWeapon> WeaponClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    TObjectPtr<class ABBWeapon> CurrentWeapon;

};
