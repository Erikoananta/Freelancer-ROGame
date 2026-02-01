// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Vehicle/ROVehiclePawnBase.h"
#include "ROVehicleOffroadCar.generated.h"

class UMaterialInstanceDynamic;
/**
 * 
 */
UCLASS()
class ROGAME_API AROVehicleOffroadCar : public AROVehiclePawnBase
{
	GENERATED_BODY()
	
public:
	AROVehicleOffroadCar();

protected:
	/** Chassis static mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Chassis;

	/** FL Tire static mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TireFrontLeft;

	/** FR Tire static mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TireFrontRight;

	/** RL Tire static mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TireRearLeft;

	/** RR Tire static mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TireRearRight;

protected:
	virtual void BeginPlay() override;

	virtual void ApplyVehicleDamage(
		EVehicleDamageSide Side,
		const FVector& HitLocationWS
	) override;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ChassisMID;

private:
	void TestDeform();

};
