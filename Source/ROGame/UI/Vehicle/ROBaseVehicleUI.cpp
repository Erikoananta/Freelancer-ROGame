// Fill out your copyright notice in the Description page of Project Settings.


#include "ROBaseVehicleUI.h"

void UROBaseVehicleUI::UpdateSpeed(float NewSpeed)
{
	// format the speed to KPH or MPH
	float FormattedSpeed = FMath::Abs(NewSpeed) * (bIsMPH ? 0.022f : 0.036f);

	// call the Blueprint handler
	OnSpeedUpdate(FormattedSpeed);
}

void UROBaseVehicleUI::UpdateGear(int32 NewGear)
{
	// call the Blueprint handler
	OnGearUpdate(NewGear);
}

void UROBaseVehicleUI::UpdateRPM(float NewRPM)
{
	// call the Blueprint handler
	OnRPMUpdate(NewRPM);
}
