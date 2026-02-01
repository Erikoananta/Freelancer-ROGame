// Fill out your copyright notice in the Description page of Project Settings.


#include "ROVehiclePawnBase.h"

/* CUSTOM */
#include "../Components/InteractableComponent.h"
#include "../Components/ROAttributeComponent.h"
#include "../Character/ROGameCharacter.h"
#include "../UI/Vehicle/ROBaseVehicleUI.h"


/* ENGINE */
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

AROVehiclePawnBase::AROVehiclePawnBase()
{
	// Initialize Interactable Component
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
	InteractableComponent->SetupAttachment(GetMesh()); // Attach to the vehicle mesh

	// construct the front camera boom
	FrontSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Front Spring Arm"));
	FrontSpringArm->SetupAttachment(GetMesh());
	FrontSpringArm->TargetArmLength = 0.0f;
	FrontSpringArm->bDoCollisionTest = false;
	FrontSpringArm->bEnableCameraRotationLag = true;
	FrontSpringArm->CameraRotationLagSpeed = 15.0f;
	FrontSpringArm->SetRelativeLocation(FVector(30.0f, 0.0f, 120.0f));

	FrontCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Front Camera"));
	FrontCamera->SetupAttachment(FrontSpringArm);
	FrontCamera->bAutoActivate = false;

	// construct the back camera boom
	BackSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Back Spring Arm"));
	BackSpringArm->SetupAttachment(GetMesh());
	BackSpringArm->TargetArmLength = 650.0f;
	BackSpringArm->SocketOffset.Z = 150.0f;
	BackSpringArm->bDoCollisionTest = false;
	BackSpringArm->bInheritPitch = false;
	BackSpringArm->bInheritRoll = false;
	BackSpringArm->bEnableCameraRotationLag = true;
	BackSpringArm->CameraRotationLagSpeed = 2.0f;
	BackSpringArm->CameraLagMaxDistance = 50.0f;
	BackSpringArm->bUsePawnControlRotation = true;

	BackCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Back Camera"));
	BackCamera->SetupAttachment(BackSpringArm);

	// Configure the car mesh
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(FName("Vehicle"));

	// Ensure the mesh notifies on rigid body collisions
	GetMesh()->SetNotifyRigidBodyCollision(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(FName("Vehicle"));

	FrontSensor = CreateDefaultSubobject<UBoxComponent>(TEXT("FrontSensor"));
	FrontSensor->SetupAttachment(GetMesh());
	FrontSensor->SetCollisionProfileName("OverlapAllDynamic");

	RearSensor = CreateDefaultSubobject<UBoxComponent>(TEXT("RearSensor"));
	RearSensor->SetupAttachment(GetMesh());
	RearSensor->SetCollisionProfileName("OverlapAllDynamic");

	LeftSensor = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftSensor"));
	LeftSensor->SetupAttachment(GetMesh());
	LeftSensor->SetCollisionProfileName("OverlapAllDynamic");

	RightSensor = CreateDefaultSubobject<UBoxComponent>(TEXT("RightSensor"));
	RightSensor->SetupAttachment(GetMesh());
	RightSensor->SetCollisionProfileName("OverlapAllDynamic");

	// get the Chaos Wheeled movement component
	ChaosVehicleMovement = CastChecked<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement());
	AttributeComponent = CreateDefaultSubobject<UROAttributeComponent>(TEXT("AttributeComponent"));
}

void AROVehiclePawnBase::BeginPlay()
{
	Super::BeginPlay();

	// Bind the interaction event
	if (InteractableComponent)
	{
		InteractableComponent->OnInteract.AddDynamic(this, &AROVehiclePawnBase::HandleEnterVehicle);
	}

	if (FrontSensor && RearSensor && LeftSensor && RightSensor)
	{
		FrontSensor->OnComponentBeginOverlap.AddDynamic(this, &AROVehiclePawnBase::OnFrontSensorHit);
		RearSensor->OnComponentBeginOverlap.AddDynamic(this, &AROVehiclePawnBase::OnRearSensorHit);
		LeftSensor->OnComponentBeginOverlap.AddDynamic(this, &AROVehiclePawnBase::OnLeftSensorHit);
		RightSensor->OnComponentBeginOverlap.AddDynamic(this, &AROVehiclePawnBase::OnRightSensorHit);
	}
}

void AROVehiclePawnBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Vehicle-specific per-frame logic can go here
	InterpsToOriginRotation(DeltaTime);

	if (IsValid(VehicleUIInstance))
	{
		VehicleUIInstance->UpdateSpeed(GetChaosVehicleMovement()->GetForwardSpeed());
		VehicleUIInstance->UpdateGear(GetChaosVehicleMovement()->GetCurrentGear());
		VehicleUIInstance->UpdateRPM(GetChaosVehicleMovement()->GetEngineRotationSpeed());
	}
}

void AROVehiclePawnBase::DoThrottle(float Value)
{
	// add the input
	ChaosVehicleMovement->SetThrottleInput(Value);
	ChaosVehicleMovement->UpdatedPrimitive->WakeAllRigidBodies();

	// UE_LOG(LogTemp, Log, TEXT("Throttle Input In Pawn: %f"), Value);

	// reset the brake input
	// ChaosVehicleMovement->SetBrakeInput(0.0f);
}

void AROVehiclePawnBase::DoSteering(float Value)
{
	// add the input
	ChaosVehicleMovement->SetSteeringInput(Value);
}

void AROVehiclePawnBase::DoBrake(float Value)
{
	// add the input
	ChaosVehicleMovement->SetBrakeInput(Value);

	// reset the throttle input
	ChaosVehicleMovement->SetThrottleInput(0.0f);
}

void AROVehiclePawnBase::DoStartBrake()
{
	// call the Blueprint hook for the brake lights
	BrakeLights(true);
}

void AROVehiclePawnBase::DoStopBrake()
{
	// call the Blueprint hook for the brake lights
	BrakeLights(false);

	// reset brake input to zero
	ChaosVehicleMovement->SetBrakeInput(0.0f);
}

void AROVehiclePawnBase::SetHandbrakeInput(bool bEnabled)
{
	if (bEnabled)
	{
		// add the input
		ChaosVehicleMovement->SetHandbrakeInput(true);

		// call the Blueprint hook for the break lights
		BrakeLights(true);
	}
	else
	{
		// add the input
		ChaosVehicleMovement->SetHandbrakeInput(false);

		// call the Blueprint hook for the break lights
		BrakeLights(false);
	}
}

void AROVehiclePawnBase::LookAround(FVector2D LookInput)
{
	if (!BackSpringArm) return;

	//FRotator CurrentRot = BackSpringArm->GetRelativeRotation();

	//float NewPitch = CurrentRot.Pitch + LookInput.Y;
	//float NewYaw = CurrentRot.Yaw + LookInput.X;

	//// clamp pitch biar kamera tidak muter kebalik
	//NewPitch = FMath::Clamp(NewPitch, -60.f, 60.f);

	//BackSpringArm->SetRelativeRotation(FRotator(NewPitch, NewYaw, 0.f));
	// UE_LOG(LogTemp, Log, TEXT("Look Input In Pawn: X=%f, Y=%f"), LookInput.X, LookInput.Y);

	//BackSpringArm->AddLocalRotation(FRotator(LookInput.X, LookInput.Y, 0.0f));

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void AROVehiclePawnBase::ToggleCamera()
{
	if (bIsFrontCameraActive)
	{
		// Switch to back camera
		if (BackSpringArm && BackCamera && FrontSpringArm && FrontCamera)
		{
			FrontSpringArm->Deactivate();
			FrontCamera->Deactivate();
			BackSpringArm->Activate();
			BackCamera->Activate();
			bIsFrontCameraActive = false;
		}
	}
	else
	{
		// Switch to front camera
		if (BackSpringArm && BackCamera && FrontSpringArm && FrontCamera)
		{
			BackSpringArm->Deactivate();
			BackCamera->Deactivate();
			FrontSpringArm->Activate();
			FrontCamera->Activate();
			bIsFrontCameraActive = true;
		}
	}
}

void AROVehiclePawnBase::ResetPositionAndCamera()
{
	// reset to a location slightly above our current one
	FVector ResetLocation = GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);

	// reset to our yaw. Ignore pitch and roll
	FRotator ResetRotation = GetActorRotation();
	ResetRotation.Pitch = 0.0f;
	ResetRotation.Roll = 0.0f;

	// teleport the actor to the reset spot and reset physics
	SetActorTransform(FTransform(ResetRotation, ResetLocation, FVector::OneVector), false, nullptr, ETeleportType::TeleportPhysics);

	GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);

	UE_LOG(LogTemp, Log, TEXT("Vehicle reset successfully with Teleport flag."));
}

void AROVehiclePawnBase::InterpsToOriginRotation(float DeltaTime)
{
	// add some angular damping if the vehicle is in midair
	bool bMovingOnGround = ChaosVehicleMovement->IsMovingOnGround();
	GetMesh()->SetAngularDamping(bMovingOnGround ? 0.0f : 3.0f);

	// realign the camera yaw to face front
	float CameraYaw = BackSpringArm->GetRelativeRotation().Yaw;
	CameraYaw = FMath::FInterpTo(CameraYaw, 0.0f, DeltaTime, 1.0f);

	BackSpringArm->SetRelativeRotation(FRotator(0.0f, CameraYaw, 0.0f));
}

void AROVehiclePawnBase::FlippedCheck()
{
	// Dot Product < 0 artinya sudut antara atap mobil dan langit sudah lebih dari 90 derajat
	// Menggunakan < -0.2f memberikan toleransi agar tidak memicu reset saat mobil hanya miring ekstrem
	float DotUp = FVector::DotProduct(GetMesh()->GetUpVector(), FVector::UpVector);
	bool bIsFlipped = DotUp < -0.2f;

	if (bIsFlipped)
	{
		if (bPreviousFlipCheck)
		{
			ResetPositionAndCamera();
			bPreviousFlipCheck = false; // Reset flag setelah eksekusi
		}
		else
		{
			bPreviousFlipCheck = true; // Menunggu konfirmasi di check berikutnya (10 detik lagi)
		}
	}
	else
	{
		bPreviousFlipCheck = false;
	}
}


void AROVehiclePawnBase::HandleEnterVehicle(AROGameCharacter* Interactor)
{
	if (!Interactor || DriverCharacter) return; // DriverCharacter check to prevent multiple drivers

	APlayerController* PC = Cast<APlayerController>(Interactor->GetController());
	if (PC)
	{
		DriverCharacter = Interactor;

		// 1. Disable character collision and physics
		DriverCharacter->SetActorEnableCollision(false);

		// 2. Attach character to vehicle and hide
		DriverCharacter->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		DriverCharacter->SetActorHiddenInGame(true);
		if (InteractableComponent)
		{
			InteractableComponent->SetIsInteractable(false);
		}

		// 3. Possess the vehicle
		PC->Possess(this);

		// Syntax: TimerHandle, Object, Function Address, Rate (Seconds), Looping?, Initial Delay
		GetWorldTimerManager().SetTimer(TimerHandle_FlippedCheck, this, &AROVehiclePawnBase::FlippedCheck, 10.0f, true);


		// Spawn UI HANYA jika class-nya sudah dipilih di Blueprint
		if (VehicleUIClass && PC->IsLocalController())
		{
			VehicleUIInstance = CreateWidget<UROBaseVehicleUI>(PC, VehicleUIClass);
			if (VehicleUIInstance)
			{
				VehicleUIInstance->AddToViewport();
				UE_LOG(LogTemp, Log, TEXT("Vehicle UI Added to Viewport"));
			}
		}

		UE_LOG(LogTemp, Log, TEXT("Player possessed vehicle: %s"), *GetName());
	}
}

void AROVehiclePawnBase::ExitVehicle()
{
	if (!DriverCharacter) return;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		// 1. Unhide and detach character
		DriverCharacter->SetActorHiddenInGame(false);
		DriverCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// 2. Position character next to the car (Adjust offset as needed)
		FVector ExitLocation = GetActorLocation() + (GetActorRightVector() * -200.0f) + FVector(0, 0, 100);
		DriverCharacter->SetActorLocation(ExitLocation);

		// 3. Re-enable collision
		DriverCharacter->SetActorEnableCollision(true);

		// 4. Possess character back
		PC->Possess(DriverCharacter);

		DriverCharacter = nullptr;

		// Re-enable interaction so the car can be driven again
		if (InteractableComponent)
		{
			InteractableComponent->SetIsInteractable(true);
		}

		GetWorldTimerManager().ClearTimer(TimerHandle_FlippedCheck);

		// Hapus UI sebelum keluar
		if (VehicleUIInstance)
		{
			VehicleUIInstance->RemoveFromParent();
			VehicleUIInstance = nullptr; // Membersihkan referensi agar di-Garbage Collect
			UE_LOG(LogTemp, Log, TEXT("Vehicle UI Removed from Viewport"));
		}
	}
}

void AROVehiclePawnBase::OnFrontSensorHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherComp || OtherActor == this) return;

	FVector HitLocation;
	// Fungsi ini mengembalikan float (jarak), parameter ke-3 adalah FName untuk tulang (opsional)
	float Distance = OtherComp->GetClosestPointOnCollision(OverlappedComponent->GetComponentLocation(), HitLocation);

	// Jika titik tidak ditemukan (jarak negatif), gunakan posisi sensor sebagai cadangan
	if (Distance < 0.0f) HitLocation = OverlappedComponent->GetComponentLocation();

	ApplyVehicleDamage(EVehicleDamageSide::Front, HitLocation);
}

void AROVehiclePawnBase::OnRearSensorHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherComp || OtherActor == this) return;

	FVector HitLocation;
	float Distance = OtherComp->GetClosestPointOnCollision(OverlappedComponent->GetComponentLocation(), HitLocation);

	if (Distance < 0.0f) HitLocation = OverlappedComponent->GetComponentLocation();

	ApplyVehicleDamage(EVehicleDamageSide::Rear, HitLocation);
}

void AROVehiclePawnBase::OnLeftSensorHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherComp || OtherActor == this) return;

	FVector HitLocation;
	float Distance = OtherComp->GetClosestPointOnCollision(OverlappedComponent->GetComponentLocation(), HitLocation);

	if (Distance < 0.0f) HitLocation = OverlappedComponent->GetComponentLocation();

	ApplyVehicleDamage(EVehicleDamageSide::Left, HitLocation);
}

void AROVehiclePawnBase::OnRightSensorHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherComp || OtherActor == this) return;

	FVector HitLocation;
	float Distance = OtherComp->GetClosestPointOnCollision(OverlappedComponent->GetComponentLocation(), HitLocation);

	if (Distance < 0.0f) HitLocation = OverlappedComponent->GetComponentLocation();

	ApplyVehicleDamage(EVehicleDamageSide::Right, HitLocation);
}

void AROVehiclePawnBase::ApplyVehicleDamage(EVehicleDamageSide Side, const FVector& HitLocationWS)
{
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
}


