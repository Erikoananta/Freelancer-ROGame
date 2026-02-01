// Fill out your copyright notice in the Description page of Project Settings.


#include "ROVehicleWheelFrontBase.h"

UROVehicleWheelFrontBase::UROVehicleWheelFrontBase()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	MaxSteerAngle = 40.f;
}
