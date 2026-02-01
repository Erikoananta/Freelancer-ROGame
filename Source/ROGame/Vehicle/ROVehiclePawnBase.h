// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "ROVehiclePawnBase.generated.h"


class UInteractableComponent;
class UROAttributeComponent;
class AROGameCharacter;
class UChaosWheeledVehicleMovementComponent;
class UROBaseVehicleUI;
class UBoxComponent;

// Letakkan UENUM di atas class
UENUM(BlueprintType)
enum class EVehicleDamageSide : uint8
{
	Front,
	Rear,
	Left,
	Right
};

/**
 * 
 */
UCLASS()
class ROGAME_API AROVehiclePawnBase : public AWheeledVehiclePawn
{
	GENERATED_BODY()
	
public:
	AROVehiclePawnBase();
	virtual void Tick(float DeltaTime) override;
	

	// Input Bridge Functions
	void DoThrottle(float Value);
	void DoSteering(float Value);
	void DoBrake(float Value);
	void DoStartBrake();
	void DoStopBrake();
	void SetHandbrakeInput(bool bEnabled);
	void LookAround(FVector2D LookInput);
	void ToggleCamera();
	void ResetPositionAndCamera();
protected:
	virtual void BeginPlay() override;

	/** Called when player interacts with this vehicle */
	UFUNCTION()
	void HandleEnterVehicle(class AROGameCharacter* Interactor);

	/** Called when the brake lights are turned on or off */
	UFUNCTION(BlueprintImplementableEvent, Category = "Vehicle")
	void BrakeLights(bool bBraking);

	UFUNCTION()
	void OnFrontSensorHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnRearSensorHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnLeftSensorHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnRightSensorHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// === MAIN DAMAGE FUNCTION ===
	virtual void ApplyVehicleDamage(
		EVehicleDamageSide Side,
		const FVector& HitLocationWS
	);
protected:

	/** Component to handle player interaction */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInteractableComponent> InteractableComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> FrontSensor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> RearSensor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> LeftSensor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> RightSensor;

	/** Camera Components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class USpringArmComponent> BackSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UCameraComponent> BackCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class USpringArmComponent> FrontSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UCameraComponent> FrontCamera;
	/** END Camera Components */

	/** Track which camera is currently active */
	bool bIsFrontCameraActive = false;

	/** Reference to the vehicle UI */
	/** Reference to the UI Widget Class to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UROBaseVehicleUI> VehicleUIClass;

	/** The actual instance of the UI */
	UPROPERTY()
	TObjectPtr<UROBaseVehicleUI> VehicleUIInstance;

	/** Reference to the character currently driving */
	UPROPERTY(BlueprintReadOnly, Category = "Vehicle")
	TObjectPtr<AROGameCharacter> DriverCharacter;

	/** Cast pointer to the Chaos Vehicle movement component */
	UPROPERTY(BlueprintReadOnly, Category = "Vehicle")
	TObjectPtr<UChaosWheeledVehicleMovementComponent> ChaosVehicleMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UROAttributeComponent> AttributeComponent;
private:
	bool bPreviousFlipCheck = false;
	FTimerHandle TimerHandle_FlippedCheck;
	/** Check If Car Is Flip Or Not*/
	void FlippedCheck();
	/** Update Car Rotation To center */
	void InterpsToOriginRotation(float DeltaTime);
public:
	FORCEINLINE UChaosWheeledVehicleMovementComponent* GetChaosVehicleMovement() const { return ChaosVehicleMovement; }
	FORCEINLINE USpringArmComponent* GetBackSpringArm() const { return BackSpringArm; }
	FORCEINLINE UCameraComponent* GetBackCamera() const { return BackCamera; }
	FORCEINLINE USpringArmComponent* GetFrontSpringArm() const { return FrontSpringArm; }
	FORCEINLINE UCameraComponent* GetFrontCamera() const { return FrontCamera; }


	/** Function to exit the vehicle */
	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	void ExitVehicle();

};
