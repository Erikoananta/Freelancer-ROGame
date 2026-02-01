// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "ROGamePlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;
class UInputAction;
class AROGameCharacter;
class AROVehiclePawnBase;

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class AROGamePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AROGamePlayerController();

	// This is the correct place to declare OnPossess
	virtual void OnPossess(APawn* InPawn) override;
protected:
	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

protected:
	/* INPUT ACTION : HUMAN*/
	UPROPERTY(EditAnywhere, Category = "Input | Human")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input | Human")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Input | Human")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input | Human")
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, Category = "Input | Human")
	UInputAction* AttackAction;
	/* END INPUT ACTION : HUMAN*/

	/* INPUT ACTION : VEHICLE */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Vehicle")
	TObjectPtr<UInputAction> IA_Throttle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Vehicle")
	TObjectPtr<UInputAction> IA_Steering;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Vehicle")
	TObjectPtr<UInputAction> IA_Brake;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Vehicle")
	TObjectPtr<UInputAction> IA_Handbrake;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Vehicle")
	TObjectPtr<UInputAction> IA_Headlights;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Vehicle")
	TObjectPtr<UInputAction> IA_ToggleCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Vehicle")
	TObjectPtr<UInputAction> IA_LookAround;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input | Vehicle")
	TObjectPtr<UInputAction> IA_ExitVehicle;
	/* END INPUT ACTION : VEHICLE */

	/** Human Input Handlers */
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void JumpStarted();
	void JumpStopped();
	void InteractObject();
	void AttackStarted();
	void AttackStopped();

	/** Vehicle Input Handlers */
	void VehicleThrottle(const FInputActionValue& Value);
	void VehicleSteering(const FInputActionValue& Value);
	void VehicleBrake(const FInputActionValue& Value);
	void VehicleStartBrake(const FInputActionValue& Value);
	void VehicleStopBrake(const FInputActionValue& Value);
	void VehicleHandbrakeStarted();
	void VehicleHandbrakeStopped();
	void VehicleToggleHeadlights();
	void VehicleToggleCamera();
	void VehicleLookAround(const FInputActionValue& Value);
	void RequestExitVehicle();
protected:
	/** Contexts added automatically on start (e.g., UI, Chat) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TArray<TObjectPtr<UInputMappingContext>> DefaultMappingContexts;

	/** Human-specific context */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> HumanMappingContext;

	/** Vehicle-specific context */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> VehicleMappingContext;

	// ... Input Actions (IA_Throttle, IA_Move, etc) ...

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category = "Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	TObjectPtr<UUserWidget> MobileControlsWidget;

private:
	UPROPERTY()
	TObjectPtr<AROGameCharacter> ControlledHumanCharacter;

	UPROPERTY()
	TObjectPtr<AROVehiclePawnBase> ControlledVehiclePawn;
};
