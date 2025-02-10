// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bloodborne.h"
#include "BBCharacter.h"
#include "HunterCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

//Consider bitflag
UENUM(BlueprintType)
enum class EMovementState : uint8
{
    None UMETA(DisplayName = "None"), 
    Walk UMETA(DisplayName = "Walk"),
    Run UMETA(DisplayName = "Run"),
    Sprint UMETA(DisplayName = "Sprint"),
    Roll UMETA(DisplayName = "Roll"),
    Dodge UMETA(DisplayName = "Dodge"),
    Backstep UMETA(DisplayName = "Backstep")
};

UENUM(BlueprintType)
enum class EActionType : uint8
{
    None UMETA(DisplayName = "None"),
    LightAttack UMETA(DisplayName = "LightAttack"),
    HeavyAttack UMETA(DisplayName = "HeavyAttack"),
    WeaponChange UMETA(DisplayName = "WeaponChange"),
    LeftAttack UMETA(DisplayName = "LeftAttack"),

    UseItem UMETA(DisplayName = "UseItem"),
    Heal UMETA(DisplayName = "Heal"),
    Interact UMETA(DisplayName = "Interact"),
};

UENUM(BlueprintType)
enum class EWeaponForm : uint8
{
    Regular UMETA(DisplayName = "Regular"),
    Transformed UMETA(DisplayName = "Transformed")
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
    TObjectPtr<class UHunterAnimInstance> Anim;

//Normal / Movement
public:
    void Move(const FVector2D& Vector);
    void MoveEnd();
    void Look(const FVector2D& Vector);
    void CustomJump();
    void CustomStopJump();
    void Sprint();
    void StopSprinting();
    void Dodge();
    void StopDodging();

    // 입력 버퍼 저장 (비어있을 때만 저장)
    template <typename Func>
    void BindBufferedAction(Func&& Action)
    {
        if (bCanInput && !bCanNextAction)
        {
            if (!HasBufferedAction())
                BufferedAction = [Action]() { Action(); };
        }
        else if (bCanNextAction)
        {
            Action();
        }
    }

    // 버퍼 확인
    bool HasBufferedAction() const;

    bool GetbHasMovementInput() const;
    void SetbHasMovementInput(bool input);

    bool GetIsSprinting() const;
    bool GetIsDodging() const;
    float GetDirectionAngle();
    void SetDirectionAngle(FVector2D Vector);

    bool GetbCanInput() const;
    bool GetbCanNextAction() const;
    void SetbCanInput(bool input);
    void SetbCanNextAction(bool input);

    //private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector2D InputDirection = FVector2D::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool bHasMovementInput = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float InputChangeRate = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float InputIntensity = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool bIsSprinting = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool bIsDodging = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool bCanInput = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool bCanNextAction = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MovementDirectionAngle = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float targetSpeed = 0.0f;

    TFunction<void()> BufferedAction;

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
    bool bIsLockOn = false;

    TObjectPtr<APawn> FindClosestPawn();

private:
    UPROPERTY(EditAnywhere, Category = "Camera")
    float MaxLockOnDistance = 1000.0f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    TObjectPtr<APawn> TargetPawn = nullptr;

//MovementState

public:
    EMovementState GetMovementState();
    void SetMovementState(EMovementState State);

    //EActionType GetActionType() const;
    //void SetActionType(EActionType Action);

private:
    class MovementState
    {
    public:
        virtual void Move(AHunterCharacter* Chr) {};
        virtual void HandleAction(AHunterCharacter* Chr) = 0;
        void NormalAction(AHunterCharacter* Chr);

        virtual ~MovementState() = default;
    };

    class NoneState : public MovementState
    {
        void Move(AHunterCharacter* Chr) override;
        void HandleAction(AHunterCharacter* Chr) override;
    };

    class WalkState : public MovementState
    {
        void Move(AHunterCharacter* Chr) override;
        void HandleAction(AHunterCharacter* Chr) override;
    };

    class RunState : public MovementState
    {
        void Move(AHunterCharacter* Chr) override;
        void HandleAction(AHunterCharacter* Chr) override;
    };

    class SprintState : public MovementState
    {
        void Move(AHunterCharacter* Chr) override;
        void HandleAction(AHunterCharacter* Chr) override;
    };

    class RollState : public MovementState
    {
        void HandleAction(AHunterCharacter* Chr) override;
    };

    class DodgeState : public MovementState
    {
        void HandleAction(AHunterCharacter* Chr) override;
    };

    class BackstepState : public MovementState
    {
        void HandleAction(AHunterCharacter* Chr) override;
    };

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    EMovementState ECurrentMovementState;
    //UPROPERTY
    MovementState* CurrentMovementState;

    std::unique_ptr<NoneState> noneState = std::make_unique<NoneState>(); 
    std::unique_ptr<WalkState> walkState = std::make_unique<WalkState>();
    std::unique_ptr<RunState> runState = std::make_unique<RunState>();
    std::unique_ptr<SprintState> sprintState = std::make_unique<SprintState>();
    std::unique_ptr<RollState> rollState = std::make_unique<RollState>();
    std::unique_ptr<DodgeState> dodgeState = std::make_unique<DodgeState>();
    std::unique_ptr<BackstepState> backstepState = std::make_unique<BackstepState>();

//Weapon
public:

    //For Inventory Weapon Change, Modify Later
    //Seperate Set & Remove
    void SetRightWeapon(TObjectPtr<class ABBWeapon>& RightWeapon, FName WeaponName);
    void SetLeftWeapon(TObjectPtr<class ABBWeapon*>&, FName WeaponName);

    class ABBWeapon* GetRightWeapon();

    UDataTable* GetWeaponDataTable() const;

    EActionType GetActionType() const;
    EWeaponForm GetWeaponForm() const;
    //RemoveWeapon()

    bool GetbIsCharging() const;
    void SetbIsCharging(bool input);

    bool GetbCanQuitCharge() const;
    void SetbCanQuitCharge(bool input);


private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    UDataTable* WeaponDataTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<class ABBWeapon> WeaponClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class UWeaponManager> WeaponManager;

    FName RightWeaponSocket;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class ABBWeapon> CurrentRWeapon;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class ABBWeapon> RWeapon1;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class ABBWeapon> RWeapon2;
    
    bool bIsCharging = false;
    bool bCanQuitCharge = false;



    FName LeftWeaponSocket;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class ABBWeapon> CurrentLWeapon;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class ABBWeapon> LWeapon1;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class ABBWeapon> LWeapon2;
   
    FName WeaponName1 = FName(TEXT("SawCleaver"));

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    EActionType ECurrentActionType;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    EWeaponForm ECurrentWeaponForm;

//RightWeaponState
private:
    //Section odd / even -> right / left


    //split 0, 1 to left and right
    //add new notify for combo attack montage that can hold combo index
    //+ add new method for end delegate that make combo index = 0
    // make case for that combo systems

    //Right to Left : even    /   Left to Right : odd

//Attack & Damage
//Flow: 1. playercontroller calls Hunter -> Lef / Hea / WeaChan Action
//      2. Hunter calls HandleAction by its movement state (roll, sprint ...)
//      3. HandleAction calls weaponstate's method
public:
    
    //virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    void LightAttack();
    void HeavyAttack();
    void WeaponChange();
    //void AttackCheck();

    void HeavyAttackEnd();
    void ChargeAttackEnd();

    UFUNCTION()
    void OnMontageStarted(UAnimMontage* Montage);

    UFUNCTION()
    void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
    bool IsAttacking = false;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
    bool IsComboInputOn = false;
};
