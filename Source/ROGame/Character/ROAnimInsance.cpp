// Fill out your copyright notice in the Description page of Project Settings.


#include "ROAnimInsance.h"

/* CUSTOM */
#include "ROGameCharacter.h"

/* ENGINE */
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"

void UROAnimInsance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	InitializeReferences();
}

void UROAnimInsance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (ROCharacter)
	{
		if (MovementComponent)
		{
			Velocity = MovementComponent->Velocity;
			GroundSpeed = Velocity.Size2D(); 
			float BaseDirection = UKismetAnimationLibrary::CalculateDirection(Velocity, ROCharacter->GetActorRotation());
			Direction = MovementComponent->bOrientRotationToMovement ? FMath::Clamp(BaseDirection, -45.f, 45.f) : BaseDirection;
			bShouldMove = GroundSpeed > 3.f;
			bIsFalling = MovementComponent->IsFalling();
			HandlingWeaponMode = ROCharacter->GetCurrentHandlingWeaponMode();
			AimPitch = ROCharacter->GetAimPitch();
			AimYaw = ROCharacter->GetAimYaw();
			LeftHandIKTransform = ROCharacter->GetLeftHandTargetTransform();

			// UE_LOG
			// UE_LOG(LogTemp, Log, TEXT("AimPitch: %f, AimYaw: %f"), AimPitch, AimYaw);
		}
	}
	else
	{
		InitializeReferences();
	}
}

void UROAnimInsance::InitializeReferences()
{
	if (!ROCharacter)
	{
		APawn* OwningPawn = TryGetPawnOwner();
		if (OwningPawn)
		{
			ROCharacter = Cast<AROGameCharacter>(OwningPawn);
			if (ROCharacter)
			{
				MovementComponent = ROCharacter->GetCharacterMovement();
			}
		}
	}
}
