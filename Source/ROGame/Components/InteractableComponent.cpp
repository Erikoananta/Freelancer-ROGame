// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableComponent.h"

#include "../ROGame.h"

UInteractableComponent::UInteractableComponent()
{
	SphereRadius = 100.f;
	// Pastikan collision defaultnya memblokir Visibility trace
	// Sekarang komponen ini secara fisik adalah "Interactable"
	SetCollisionObjectType(ECC_InteractObject);

	SetCollisionResponseToAllChannels(ECR_Ignore);

	// Dan hanya merespons sinar laser dari channel "Interact"
	SetCollisionResponseToChannel(ECC_InteractTrace, ECR_Block);
}

void UInteractableComponent::Interact(AROGameCharacter* Interactor)
{
	if (OnInteract.IsBound())
	{
		OnInteract.Broadcast(Interactor);
	}
}

void UInteractableComponent::SetIsFocus(bool bIsFocused)
{
	if (bIsFocused)
	{
		OnBeginFocus.Broadcast(); // Notify listeners that focus started
	}
	else
	{
		OnEndFocus.Broadcast(); // Notify listeners that focus ended
	}
}

void UInteractableComponent::SetIsInteractable(bool bNewState)
{
	bIsInteractable = bNewState;

	if (bIsInteractable)
	{
		// Re-enable collision or specific detection logic
		SetComponentTickEnabled(true);
	}
	else
	{
		// Stop the component from wasting CPU cycles
		SetComponentTickEnabled(false);
		// Hide any interaction widgets/icons if you have them
	}
}
