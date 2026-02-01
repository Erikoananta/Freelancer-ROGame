// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ROAttributeComponent.h"

/* CUSTOM */

/* ENGINE */

UROAttributeComponent::UROAttributeComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

	Health = MaxHealth;
}

void UROAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UROAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UROAttributeComponent::ApplyHealthChange(float Delta, const UDamageType* DamageType)
{
	float OldHealth = Health;
	Health = FMath::Clamp(Health + Delta, 0.0f, MaxHealth);

	// Jika ada perubahan, panggil event
	if (Health != OldHealth)
	{
		OnHealthChanged.Broadcast(this, Health, MaxHealth, DamageType);
	}

	return Health <= 0.0f && OldHealth > 0.0f; // Mengembalikan true jika baru saja mati
}

