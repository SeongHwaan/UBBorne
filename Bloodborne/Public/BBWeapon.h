// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBWeapon.generated.h"

UCLASS()
class BLOODBORNE_API ABBWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABBWeapon();

    virtual float GetAttackDamage() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


    UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadWrite, Category = "Attack")
    float AttackDamage;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, Category = "Weapon")
    TObjectPtr<USkeletalMeshComponent> Weapon;

};
