// Fill out your copyright notice in the Description page of Project Settings.


#include "BBWeapon.h"
#include "WeaponInstance.h"

// Sets default values
ABBWeapon::ABBWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
    RootComponent = WeaponMesh;

    //Weapon->SetCollisionProfileName(TEXT("NoCollision"));
}


const TObjectPtr<class UWeaponInstance> ABBWeapon::GetWeaponInstance()
{
    return WeaponInstance;
}

void ABBWeapon::SetWeaponInstance(UWeaponInstance* Instance)
{
    WeaponInstance = Instance;
}

void ABBWeapon::SetWeaponMesh()
{
    if (WeaponInstance != nullptr)
        WeaponMesh->SetSkeletalMesh(WeaponInstance->GetWeaponMesh());
}

// Called when the game starts or when spawned
void ABBWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABBWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

