// Fill out your copyright notice in the Description page of Project Settings.


#include "BBWeapon.h"
#include "WeaponInstance.h"
#include "BBGameInstance.h"
#include "ResourceManager.h"
#include "HunterAnimInstance.h"

// Sets default values
ABBWeapon::ABBWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    RootComponent = WeaponMeshComponent;

    //WeaponMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));

}

void ABBWeapon::Initialize()
{
    if (UWorld* World = GetWorld())
    {
        UBBGameInstance* MyGameInstance = Cast<UBBGameInstance>(World->GetGameInstance());
        auto ResourceManager = MyGameInstance->GetSubsystem<UResourceManager>();
        PlayerAnimInstance = ResourceManager->GetPlayerAnimInstance();
    }

    // Delegate in PostInitializeComponents or BeginPlay rather than a constructor.
    if (PlayerAnimInstance)
    {
        auto Anim = Cast<UHunterAnimInstance>(PlayerAnimInstance);
    }
    else
        UE_LOG(LogTemp, Warning, TEXT("no AnimInstance"));

}


TObjectPtr<class UWeaponInstance> ABBWeapon::GetWeaponInstance() const
{
    return WeaponInstance;
}

void ABBWeapon::SetWeaponInstance(UWeaponInstance* Instance)
{
    WeaponInstance = Instance;
}

void ABBWeapon::SetWeaponMeshComponent()
{
    if (WeaponInstance != nullptr)
        WeaponMeshComponent->SetSkeletalMesh(WeaponInstance->GetWeaponMesh());
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


void ABBWeapon::LightCombo(EWeaponForm Form)
{
    WeaponInstance->PlayLightCombo(Form);
}

void ABBWeapon::HeavyStart(EWeaponForm Form)
{
    WeaponInstance->PlayHeavyStart(Form);
}

void ABBWeapon::HeavyEnd(EWeaponForm Form)
{
    WeaponInstance->PlayHeavyEnd(Form);
}

void ABBWeapon::ChargeEnd(EWeaponForm Form)
{
    WeaponInstance->PlayChargeEnd(Form);
}

void ABBWeapon::HeavyAfterCharge()
{
    WeaponInstance->PlayHeavyAfterCharge();
}

void ABBWeapon::RollAttack(EActionType Action, EWeaponForm Form)
{
    WeaponInstance->PlayRollAttack(Action, Form);
}

void ABBWeapon::BackstepAttack(EActionType Action, EWeaponForm Form)
{
    WeaponInstance->PlayBackstepAttack(Action, Form);
}

void ABBWeapon::DodgeAttack(EActionType Action, EWeaponForm Form, float angle)
{
    WeaponInstance->PlayDodgeAttack(Action, Form, angle);
}

void ABBWeapon::JumpAttack(EWeaponForm Form)
{
    WeaponInstance->PlayJumpAttack(Form);
}

void ABBWeapon::SprintAttack(EActionType Action, EWeaponForm Form)
{
    WeaponInstance->PlaySprintAttack(Action, Form);
}

void ABBWeapon::FormChange(EWeaponForm Form, bool bIsAttacking)
{
    WeaponInstance->PlayFormChange(Form, bIsAttacking, WeaponMeshComponent);
}
