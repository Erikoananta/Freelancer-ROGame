// Fill out your copyright notice in the Description page of Project Settings.


#include "ROVehicleOffroadCar.h"

/* CUSTOM */
#include "Wheel/ROVehicleOffroadWheelFront.h"
#include "Wheel/ROVehicleOffroadWheelRear.h"
#include "../Components/ROAttributeComponent.h"

/* ENGINE */
#include "ChaosWheeledVehicleMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

AROVehicleOffroadCar::AROVehicleOffroadCar()
{
	// construct the mesh components
	Chassis = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chassis"));
	Chassis->SetupAttachment(GetMesh());

	// NOTE: tire sockets are set from the Blueprint class
	TireFrontLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tire Front Left"));
	TireFrontLeft->SetupAttachment(GetMesh(), FName("VisWheel_FL"));
	TireFrontLeft->SetCollisionProfileName(FName("NoCollision"));

	TireFrontRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tire Front Right"));
	TireFrontRight->SetupAttachment(GetMesh(), FName("VisWheel_FR"));
	TireFrontRight->SetCollisionProfileName(FName("NoCollision"));
	TireFrontRight->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));

	TireRearLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tire Rear Left"));
	TireRearLeft->SetupAttachment(GetMesh(), FName("VisWheel_BL"));
	TireRearLeft->SetCollisionProfileName(FName("NoCollision"));

	TireRearRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tire Rear Right"));
	TireRearRight->SetupAttachment(GetMesh(), FName("VisWheel_BR"));
	TireRearRight->SetCollisionProfileName(FName("NoCollision"));
	TireRearRight->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));

	// adjust the cameras
	GetFrontSpringArm()->SetRelativeLocation(FVector(-5.0f, -30.0f, 135.0f));
	GetBackSpringArm()->SetRelativeLocation(FVector(0.0f, 0.0f, 75.0f));

	// Note: for faster iteration times, the vehicle setup can be tweaked in the Blueprint instead

	// Set up the chassis
	GetChaosVehicleMovement()->ChassisHeight = 160.0f;
	GetChaosVehicleMovement()->DragCoefficient = 0.1f;
	GetChaosVehicleMovement()->DownforceCoefficient = 0.1f;
	GetChaosVehicleMovement()->CenterOfMassOverride = FVector(0.0f, 0.0f, 75.0f);
	GetChaosVehicleMovement()->bEnableCenterOfMassOverride = true;

	// Set up the wheels
	GetChaosVehicleMovement()->bLegacyWheelFrictionPosition = false;
	GetChaosVehicleMovement()->WheelSetups.SetNum(4);

	GetChaosVehicleMovement()->WheelSetups[0].WheelClass = UROVehicleOffroadWheelFront::StaticClass();
	GetChaosVehicleMovement()->WheelSetups[0].BoneName = FName("PhysWheel_FL");
	GetChaosVehicleMovement()->WheelSetups[0].AdditionalOffset = FVector(0.0f, 0.0f, 0.0f);

	GetChaosVehicleMovement()->WheelSetups[1].WheelClass = UROVehicleOffroadWheelFront::StaticClass();
	GetChaosVehicleMovement()->WheelSetups[1].BoneName = FName("PhysWheel_FR");
	GetChaosVehicleMovement()->WheelSetups[1].AdditionalOffset = FVector(0.0f, 0.0f, 0.0f);

	GetChaosVehicleMovement()->WheelSetups[2].WheelClass = UROVehicleOffroadWheelRear::StaticClass();
	GetChaosVehicleMovement()->WheelSetups[2].BoneName = FName("PhysWheel_BL");
	GetChaosVehicleMovement()->WheelSetups[2].AdditionalOffset = FVector(0.0f, 0.0f, 0.0f);

	GetChaosVehicleMovement()->WheelSetups[3].WheelClass = UROVehicleOffroadWheelRear::StaticClass();
	GetChaosVehicleMovement()->WheelSetups[3].BoneName = FName("PhysWheel_BR");
	GetChaosVehicleMovement()->WheelSetups[3].AdditionalOffset = FVector(0.0f, 0.0f, 0.0f);

	// Set up the engine
	// NOTE: Check the Blueprint asset for the Torque Curve
	GetChaosVehicleMovement()->EngineSetup.MaxTorque = 600.0f;
	GetChaosVehicleMovement()->EngineSetup.MaxRPM = 5000.0f;
	GetChaosVehicleMovement()->EngineSetup.EngineIdleRPM = 1200.0f;
	GetChaosVehicleMovement()->EngineSetup.EngineBrakeEffect = 0.05f;
	GetChaosVehicleMovement()->EngineSetup.EngineRevUpMOI = 5.0f;
	GetChaosVehicleMovement()->EngineSetup.EngineRevDownRate = 600.0f;

	// Set up the differential
	GetChaosVehicleMovement()->DifferentialSetup.DifferentialType = EVehicleDifferential::AllWheelDrive;
	GetChaosVehicleMovement()->DifferentialSetup.FrontRearSplit = 0.5f;

	// Set up the steering
	// NOTE: Check the Blueprint asset for the Steering Curve
	GetChaosVehicleMovement()->SteeringSetup.SteeringType = ESteeringType::AngleRatio;
	GetChaosVehicleMovement()->SteeringSetup.AngleRatio = 0.7f;
}

void AROVehicleOffroadCar::BeginPlay()
{
	Super::BeginPlay();

	// create the dynamic material instance for the chassis
	if (Chassis && Chassis->GetMaterial(0))
	{
		ChassisMID = UMaterialInstanceDynamic::Create(Chassis->GetMaterial(0), this);
		if (ChassisMID)
		{
			Chassis->SetMaterial(0, ChassisMID);
		}
	}

	// TestDeform();
}

void AROVehicleOffroadCar::ApplyVehicleDamage(EVehicleDamageSide Side, const FVector& HitLocationWS)
{
	Super::ApplyVehicleDamage(Side, HitLocationWS);

	if (!AttributeComponent) return;

	// === SPEED (cm/s → m/s) ===
	float SpeedMS = GetVelocity().Size() / 100.f;

	// Abaikan tabrakan pelan
	if (SpeedMS < 5.f) return;

	// === DAMAGE ===
	float DamageToApply = (SpeedMS - 5.f) * 5.f;
	AttributeComponent->ApplyHealthChange(-DamageToApply);


	UE_LOG(LogTemp, Warning,
		TEXT("VEHICLE HIT [%s] | Speed: %.2f m/s | Damage: %.2f | Radius: %.1f"),
		*UEnum::GetValueAsString(Side),
		SpeedMS,
		DamageToApply,
		SpeedMS * 20.f
	);

	// === VISUAL DEFORM ===
	if (ChassisMID)
	{
		// Strength (seberapa dalam)
		float DeformStrength = FMath::Clamp(
			DamageToApply * 0.3f,
			2.f,
			25.f
		);

		// Radius (seberapa luas)
		float DeformRadius = FMath::Clamp(
			SpeedMS * 20.f,
			80.f,
			250.f
		);

		FVector HitLocationLS =
			Chassis->GetComponentTransform()
			.InverseTransformPosition(HitLocationWS);

		ChassisMID->SetVectorParameterValue(
			TEXT("HitLocation"),
			FLinearColor(
				HitLocationLS.X,
				HitLocationLS.Y,
				HitLocationLS.Z,
				0.f
			)
		);

		ChassisMID->SetScalarParameterValue(
			TEXT("DeformStrength"),
			DeformStrength
		);

		ChassisMID->SetScalarParameterValue(
			TEXT("DeformRadius"),
			DeformRadius
		);

		UE_LOG(LogTemp, Warning,
			TEXT("VEHICLE DEFORM APPLIED | Strength: %.2f | Radius: %.1f | Hit Location: %s"),
			DeformStrength,
			DeformRadius,
			*HitLocationWS.ToString()
		);
	}
}

void AROVehicleOffroadCar::TestDeform()
{
	if (!ChassisMID || !Chassis) return;

	// Ambil posisi depan mobil (world space)
	FVector FrontWorldLocation =
		Chassis->GetComponentLocation()
		+ (Chassis->GetForwardVector() * 150.f);

	FVector HitLocationLS =
		Chassis->GetComponentTransform()
		.InverseTransformPosition(FrontWorldLocation);

	// Kirim ke material
	ChassisMID->SetVectorParameterValue(
		TEXT("HitLocation"),
		FLinearColor(
			HitLocationLS.X,
			HitLocationLS.Y,
			HitLocationLS.Z,
			0.f
		)
	);

	ChassisMID->SetScalarParameterValue(
		TEXT("DeformStrength"),
		25.f   // DALAM
	);

	ChassisMID->SetScalarParameterValue(
		TEXT("DeformRadius"),
		180.f  // LUAS
	);

	UE_LOG(LogTemp, Warning, TEXT("TEST DEFORM FRONT APPLIED"));
}
