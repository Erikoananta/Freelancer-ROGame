// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

/* CUSTOM */
#include "../Character/ROGameCharacter.h"
#include "../Game/ROGamePlayerController.h"
#include "../Components/InteractableComponent.h"

/* Engine */
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"

AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    // Inisialisasi Skeletal Mesh
    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    WeaponMesh->SetupAttachment(RootComponent);

    // Inisialisasi Static Mesh
    WeaponStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponStaticMesh"));
    WeaponStaticMesh->SetupAttachment(RootComponent);
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	OwningCharacter = Cast<AROGameCharacter>(GetOwner());
	OwningController = OwningCharacter ? Cast<AROGamePlayerController>(OwningCharacter->GetController()) : nullptr;
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBase::StartAttack()
{
    // Implementasi logika untuk memulai serangan
}

void AWeaponBase::StopAttack()
{
    // Implementasi logika untuk menghentikan serangan
}



