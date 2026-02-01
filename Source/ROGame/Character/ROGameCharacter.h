// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "../ROGame.h"
#include "ROGameCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class AWeaponBase;
class UInteractableComponent;
class UItemData;
class UROAttributeComponent;

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AROGameCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AROGameCharacter();

	virtual void Tick(float DeltaTime) override;
	void Interact();
	void EquipItem(UItemData* Data);

	// Freeze Character When Getting Hit Freeze Weapon
	void ApplyFreeze(float Amount);

	void Unfreeze();


protected:
	// ======================================
	// ============= Components =============
	// ======================================
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/* Attribute Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UROAttributeComponent> AttributeComponent;
	// ======================================
	// ============= Components =============
	// ======================================
	
	// ============= Tracing For Weapon =============
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionDistance = 200.f;

	// Change the focused variable type
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UInteractableComponent> FocusedInteractable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	EHandlingWeaponMode CurrentHandlingWeaponMode = EHandlingWeaponMode::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<AWeaponBase> EquippedWeapon;

	/** The calculated pitch for Aim Offset (-90 to 90) */
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float AimPitch;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float AimYaw;

	/*
	* Freeze Character When Getting Hit Freeze Weapon
	*/
	/** Current freeze amount from 0.0 to 1.0 */
	float FreezeAmount = 0.0f;

	/** Is the character currently frozen? */
	bool bIsFrozen = false;

	FTimerHandle TimerHandle_Unfreeze;

	/** How fast the freeze meter drops per second when not being hit */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status Effects")
	float FreezeDecayRate = 0.2f;

	/** Time since the last time the character was hit by freeze spray */
	float LastFreezeHitTime = 0.0f;

	/** Delay before the decay starts after being hit */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status Effects")
	float DecayDelay = 0.5f;
	/*
	* END Freeze Character When Getting Hit Freeze Weapon
	*/
	
	// Transfrom target Left hand for IK when aiming with weapon
	FTransform LeftHandTargetTransform;

protected:
	virtual void BeginPlay() override;

	// Logika untuk mendeteksi senjata di depan kamera
	void TraceForInteractable();

	/** Function to calculate rotation values every frame */
	void CalculateAimOffset();
private:
	bool bShouldTraceForInteractable = true;
public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE AWeaponBase* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE float GetAimPitch() const { return AimPitch; }
	FORCEINLINE float GetAimYaw() const { return AimYaw; }
	FORCEINLINE FTransform GetLeftHandTargetTransform() const { return LeftHandTargetTransform; }
	FORCEINLINE UROAttributeComponent* GetAttributeComponent() const { return AttributeComponent; }
	FORCEINLINE EHandlingWeaponMode GetCurrentHandlingWeaponMode() const { return CurrentHandlingWeaponMode; }
};

