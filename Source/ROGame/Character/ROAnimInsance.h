// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "../ROGame.h"
#include "ROAnimInsance.generated.h"

class AROGameCharacter;
class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class ROGAME_API UROAnimInsance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// Berjalan di Game Thread: Tempat mengambil data dari Actor
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Component")
	TObjectPtr<AROGameCharacter> ROCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Component")
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	// Runtime Variables
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector Velocity;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float Direction;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bShouldMove;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsFalling;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	EHandlingWeaponMode HandlingWeaponMode;

	UPROPERTY(BlueprintReadOnly, Category = "Aim Offset")
	float AimPitch;

	UPROPERTY(BlueprintReadOnly, Category = "Aim Offset")
	float AimYaw;

	/** Transform untuk target IK tangan kiri (World Space) */
	UPROPERTY(BlueprintReadOnly, Category = "IK")
	FTransform LeftHandIKTransform;

	/** Apakah senjata memiliki socket tangan kiri? */
	UPROPERTY(BlueprintReadOnly, Category = "IK")
	bool bHasLeftHandSocket;

private:
	void InitializeReferences();
};
