// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ROBaseVehicleUI.generated.h"

/**
 * 
 */
UCLASS()
class ROGAME_API UROBaseVehicleUI : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	/** Controls the display of speed in Km/h or MPH */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vehicle")
	bool bIsMPH = false;

public:

	/** Called to update the speed display */
	void UpdateSpeed(float NewSpeed);

	/** Called to update the gear display */
	void UpdateGear(int32 NewGear);

	/* Called to update RPM display */
	void UpdateRPM(float NewRPM);
protected:

	/** Implemented in Blueprint to display the new speed */
	UFUNCTION(BlueprintImplementableEvent, Category = "Vehicle")
	void OnSpeedUpdate(float NewSpeed);

	/** Implemented in Blueprint to display the new gear */
	UFUNCTION(BlueprintImplementableEvent, Category = "Vehicle")
	void OnGearUpdate(int32 NewGear);

	/** Implemented in Blueprint to display the new RPM */
	UFUNCTION(BlueprintImplementableEvent, Category = "Vehicle")
	void OnRPMUpdate(float NewRPM);
};
