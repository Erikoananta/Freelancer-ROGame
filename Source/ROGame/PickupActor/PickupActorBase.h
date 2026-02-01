// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupActorBase.generated.h"

class USkeletalMeshComponent;
class UStaticMeshComponent;
class UCapsuleComponent;
class UWidgetComponent;
class UInteractableComponent;
class AROGameCharacter;
class UItemData;

UCLASS()
class ROGAME_API APickupActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupActorBase();
    virtual void Tick(float DeltaTime) override;

    // Function for showing/hiding interaction widget
    void ShowInteractionWidget(bool bShow);

    /** Called when the player interacts with this weapon via InteractableComponent */
    UFUNCTION()
    void HandleInteraction(AROGameCharacter* Interactor);

	// Initialize the pickup with item data
    void InitializePickup();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    /** Called when the player begins focusing on this weapon via InteractableComponent */
    UFUNCTION()
    void HandleBeginFocus();
    /** Called when the player ends focusing on this weapon via InteractableComponent */
    UFUNCTION()
	void HandleEndFocus();
protected:
    // Komponen utama (SceneRoot)
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<USceneComponent> SceneRoot;

    // Default Weapon Mesh (Skeletal Mesh)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USkeletalMeshComponent> SkeletalMeshObject;

    // For Static Mesh (Optional, if you want to use Static Mesh instead Skeletal Mesh)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> StaticMeshObject;

    /** Custom component to handle player interaction logic */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UInteractableComponent> InteractableComponent;

    // Widget Interaksi (Muncul saat diarahkan)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UWidgetComponent* InteractionWidget;

    /** Assign the Item Data Asset in the Editor */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    TObjectPtr<UItemData> ItemData;
public:	

};
