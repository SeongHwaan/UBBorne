// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Bloodborne.h"
#include "HunterCharacter.h"
#include "GameFramework/Actor.h"
#include "BBWeapon.generated.h"

UCLASS()
class BLOODBORNE_API ABBWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABBWeapon();

    void Initialize();

    TObjectPtr<class UWeaponInstance> GetWeaponInstance() const;
    void SetWeaponInstance(class UWeaponInstance* Instance);
    void SetWeaponMeshComponent();
    void GiveWeaponComponentToInstance();

    void LightCombo(EWeaponForm Form);
    void HeavyStart(EWeaponForm Form);
    void HeavyEnd(EWeaponForm Form);
    void ChargeEnd(EWeaponForm Form);
    void HeavyAfterCharge();
    void RollAttack(EActionType Action, EWeaponForm Form);
    void BackstepAttack(EActionType Action, EWeaponForm Form);
    void DodgeAttack(EActionType Action, EWeaponForm Form, float angle);
    void JumpAttack(EWeaponForm Form);
    void SprintAttack(EActionType Action, EWeaponForm Form);
    void FormChange(EWeaponForm Form, bool bIsAttacking);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

    UPROPERTY(VisibleAnywhere, Category = "Weapon")
    USkeletalMeshComponent* WeaponMeshComponent;

    UPROPERTY(VisibleAnywhere, Category = "Weapon")
    TObjectPtr<class UWeaponInstance> WeaponInstance;

    TObjectPtr<UAnimInstance> PlayerAnimInstance;
};
