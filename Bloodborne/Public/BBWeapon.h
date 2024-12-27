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

    const TObjectPtr<class UWeaponInstance> GetWeaponInstance();
    void SetWeaponInstance(class UWeaponInstance* Instance);
    void SetWeaponMesh();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, Category = "Weapon")
    TObjectPtr<USkeletalMeshComponent> WeaponMesh;

    UPROPERTY(VisibleAnywhere, Category = "Weapon")
    TObjectPtr<class UWeaponInstance> WeaponInstance;
};
