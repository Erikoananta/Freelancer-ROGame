// Fill out your copyright notice in the Description page of Project Settings.


#include "ROVehicleWheelRearBase.h"

UROVehicleWheelRearBase::UROVehicleWheelRearBase()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
}
