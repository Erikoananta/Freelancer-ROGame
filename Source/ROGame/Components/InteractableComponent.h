// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "InteractableComponent.generated.h"

// Delegate untuk interaksi (Bisa dipanggil di Blueprint)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, class AROGameCharacter*, Interactor);
// Delegate for focus events
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFocusChanged);
/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ROGAME_API UInteractableComponent : public USphereComponent
{
	GENERATED_BODY()
	
public:
	UInteractableComponent();

	// Event yang bisa di-bind di C++ atau Blueprint
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteract OnInteract;

	/** Triggered when the player trace starts hitting this component */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnFocusChanged OnBeginFocus;

	/** Triggered when the player trace stops hitting this component */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnFocusChanged OnEndFocus;

protected:
	UPROPERTY(EditAnywhere, Category = "Interaction")
	FString InteractionMessage = TEXT("Press E to Interact");

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	bool bIsInteractable = true;

private:
	bool bPreviousFlipCheck = false;
public:
	// Dipanggil oleh Character saat menekan tombol interaksi
	void Interact(class AROGameCharacter* Interactor);

	/** Called by the character to toggle focus state */
	void SetIsFocus(bool bIsFocused);

	/** Toggle whether this component can be interacted with */
	void SetIsInteractable(bool bNewState);
};
