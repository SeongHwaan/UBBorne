// Fill out your copyright notice in the Description page of Project Settings.


#include "BBWeapon.h"

// Sets default values
ABBWeapon::ABBWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
    RootComponent = Weapon;

    //Weapon->SetCollisionProfileName(TEXT("NoCollision"));
}

float ABBWeapon::GetAttackDamage() const
{
    return 0.0f;
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

