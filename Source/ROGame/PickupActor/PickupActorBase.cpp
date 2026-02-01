// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupActor/PickupActorBase.h"

/* CUSTOM */
#include "../ROGame.h"
#include "../Character/ROGameCharacter.h"
#include "../Components/InteractableComponent.h"
#include"../Types/ItemData.h"

/* Engine */
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
// Sets default values
APickupActorBase::APickupActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    // Inisialisasi Skeletal Mesh
    SkeletalMeshObject = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshObject"));
    SkeletalMeshObject->SetupAttachment(RootComponent);

    // Inisialisasi Static Mesh
    StaticMeshObject = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponStaticMesh"));
    StaticMeshObject->SetupAttachment(RootComponent);

    // Setup Interactable Component
    InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
    InteractableComponent->SetupAttachment(RootComponent);

    // Gunakan nama yang sesuai dengan yang ada di ROGame.h
    InteractableComponent->SetCollisionProfileName(COLLISION_PROFILE_InteractableObject);


    // Interaction Widget
    InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
    InteractionWidget->SetupAttachment(SceneRoot);
    InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen); // Muncul di atas layar
    InteractionWidget->SetDrawAtDesiredSize(true);

	InitializePickup();
}

// Called when the game starts or when spawned
void APickupActorBase::BeginPlay()
{
	Super::BeginPlay();
	
    // Bind interaction and focus events
    InteractableComponent->OnInteract.AddDynamic(this, &APickupActorBase::HandleInteraction);
    InteractableComponent->OnBeginFocus.AddDynamic(this, &APickupActorBase::HandleBeginFocus);
    InteractableComponent->OnEndFocus.AddDynamic(this, &APickupActorBase::HandleEndFocus);
}

// Called every frame
void APickupActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickupActorBase::ShowInteractionWidget(bool bShow)
{
    if (InteractionWidget) InteractionWidget->SetVisibility(bShow);
}

void APickupActorBase::HandleBeginFocus()
{
	ShowInteractionWidget(true);
}

void APickupActorBase::HandleEndFocus()
{
	ShowInteractionWidget(false);
}

void APickupActorBase::HandleInteraction(AROGameCharacter* Interactor)
{
    if (Interactor && ItemData)
    {
        // Log the interaction
        UE_LOG(LogROGame, Log, TEXT("Interacting with %s, aiming for socket: %s"),
            *ItemData->ItemName, *ItemData->AttachmentSocketName.ToString());

        // Call the character to handle the equipment logic
        Interactor->EquipItem(ItemData);

        // Destroy the pickup from the world after it's "taken"
        Destroy();
    }
}

void APickupActorBase::InitializePickup()
{
    // Logic to initialize the pickup based on ItemData
    if (ItemData)
    {
        // Example: Set the mesh based on ItemData
        if (ItemData->PickupSkeletalMesh)
        {
            SkeletalMeshObject->SetSkeletalMesh(ItemData->PickupSkeletalMesh);
        }
        else if (ItemData->PickupStaticMesh)
        {
            StaticMeshObject->SetStaticMesh(ItemData->PickupStaticMesh);
        }
        // Additional initialization logic can go here
	}
}

