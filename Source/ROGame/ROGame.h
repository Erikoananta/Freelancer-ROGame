// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/** Main log category used across the project */
DECLARE_LOG_CATEGORY_EXTERN(LogROGame, Log, All);

// Trace Channel (e.g., mapped to "Interact" in Editor)
#define ECC_InteractTrace  ECC_GameTraceChannel1
#define ECC_InteractObject  ECC_GameTraceChannel2
#define ECC_Projectile    ECC_GameTraceChannel3

#define COLLISION_PROFILE_InteractableObject  "InteractableObject"

// Define weapon types for animation and logic
UENUM(BlueprintType)
enum class EHandlingWeaponMode : uint8
{
	None     UMETA(DisplayName = "No Weapon"),
	Rifle    UMETA(DisplayName = "Rifle"),
	Pistol   UMETA(DisplayName = "Pistol"),
	Shotgun  UMETA(DisplayName = "Shotgun"),
	Melee    UMETA(DisplayName = "Melee")
};