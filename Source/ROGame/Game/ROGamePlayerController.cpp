// Copyright Epic Games, Inc. All Rights Reserved.


#include "ROGamePlayerController.h"

/* Custom */
#include "../Character/ROGameCharacter.h"
#include "../Vehicle/ROVehiclePawnBase.h"
#include "../ROGame.h"
#include "../Weapon/WeaponBase.h"

/* Engine */
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "GameFramework/Character.h"

AROGamePlayerController::AROGamePlayerController()
{

}

void AROGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	ControlledHumanCharacter = Cast<AROGameCharacter>(GetPawn());


	//// only spawn touch controls on local player controllers
	//if (SVirtualJoystick::ShouldDisplayTouchInterface() && IsLocalPlayerController())
	//{
	//	// spawn the mobile controls widget
	//	MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

	//	if (MobileControlsWidget)
	//	{
	//		// add the controls to the player screen
	//		MobileControlsWidget->AddToPlayerScreen(0);

	//	} else {

	//		UE_LOG(LogROGame, Error, TEXT("Could not spawn mobile controls widget."));

	//	}

	//}
}

void AROGamePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!InPawn) return;

	// 1. Cache the pointers for fast access
	ControlledHumanCharacter = Cast<AROGameCharacter>(InPawn);
	ControlledVehiclePawn = Cast<AROVehiclePawnBase>(InPawn);

	// 2. Manage Mapping Contexts
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// Always clear old mappings first to prevent key conflicts
		Subsystem->ClearAllMappings();

		// Add Global Contexts first (priority 0)
		for (const TObjectPtr<UInputMappingContext>& Context : DefaultMappingContexts)
		{
			if (Context) Subsystem->AddMappingContext(Context, 0);
		}

		// Add Specific Context based on what we just possessed (priority 1)
		if (ControlledVehiclePawn && VehicleMappingContext)
		{
			Subsystem->AddMappingContext(VehicleMappingContext, 1);
			UE_LOG(LogROGame, Log, TEXT("Possessed Vehicle: Switching to Vehicle Context"));
		}
		else if (ControlledHumanCharacter && HumanMappingContext)
		{
			Subsystem->AddMappingContext(HumanMappingContext, 1);
			UE_LOG(LogROGame, Log, TEXT("Possessed Human: Switching to Human Context"));
		}
	}
}

void AROGamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
			{
				/*
				** HUMAN INPUT BINDINGS 
				*/
				// Moving
				EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AROGamePlayerController::Move);

				// Looking
				EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AROGamePlayerController::Look);

				// Jumping
				EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AROGamePlayerController::JumpStarted);
				EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AROGamePlayerController::JumpStopped);

				// Interacting
				EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AROGamePlayerController::InteractObject);

				// Attack Input (Started for pressing, Completed for releasing)
				EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AROGamePlayerController::AttackStarted);
				EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &AROGamePlayerController::AttackStopped);
				/*
				** END HUMAN INPUT BINDINGS 
				*/

				/*
				** VEHICLE INPUT BINDINGS
				*/
				// Throttle & Steering (Analog/Axis)
				EnhancedInputComponent->BindAction(IA_Throttle, ETriggerEvent::Triggered, this, &AROGamePlayerController::VehicleThrottle);
				EnhancedInputComponent->BindAction(IA_Throttle, ETriggerEvent::Completed, this, &AROGamePlayerController::VehicleThrottle);

				EnhancedInputComponent->BindAction(IA_Steering, ETriggerEvent::Triggered, this, &AROGamePlayerController::VehicleSteering);
				EnhancedInputComponent->BindAction(IA_Steering, ETriggerEvent::Completed, this, &AROGamePlayerController::VehicleSteering);


				EnhancedInputComponent->BindAction(IA_Brake, ETriggerEvent::Triggered, this, &AROGamePlayerController::VehicleBrake);
				EnhancedInputComponent->BindAction(IA_Brake, ETriggerEvent::Started, this, &AROGamePlayerController::VehicleStartBrake);
				EnhancedInputComponent->BindAction(IA_Brake, ETriggerEvent::Completed, this, &AROGamePlayerController::VehicleStopBrake);

				// Handbrake (Binary/Digital)
				EnhancedInputComponent->BindAction(IA_Handbrake, ETriggerEvent::Started, this, &AROGamePlayerController::VehicleHandbrakeStarted);
				EnhancedInputComponent->BindAction(IA_Handbrake, ETriggerEvent::Completed, this, &AROGamePlayerController::VehicleHandbrakeStopped);

				// Utility
				EnhancedInputComponent->BindAction(IA_Headlights, ETriggerEvent::Started, this, &AROGamePlayerController::VehicleToggleHeadlights);
				EnhancedInputComponent->BindAction(IA_ToggleCamera, ETriggerEvent::Started, this, &AROGamePlayerController::VehicleToggleCamera);
				EnhancedInputComponent->BindAction(IA_LookAround, ETriggerEvent::Triggered, this, &AROGamePlayerController::VehicleLookAround);

				EnhancedInputComponent->BindAction(IA_ExitVehicle, ETriggerEvent::Started, this, &AROGamePlayerController::RequestExitVehicle);
				/*
				** END VEHICLE INPUT BINDINGS
				*/
			}
		}
	}
}

void AROGamePlayerController::Move(const FInputActionValue& Value)
{
	if (ControlledHumanCharacter)
	{
		FVector2D MovementVector = Value.Get<FVector2D>();

		// Logic rotasi biasanya diambil dari Controller
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		ControlledHumanCharacter->AddMovementInput(ForwardDirection, MovementVector.Y);
		ControlledHumanCharacter->AddMovementInput(RightDirection, MovementVector.X);
	}
	else
	{
		ControlledHumanCharacter = Cast<AROGameCharacter>(GetPawn());
	}
}

void AROGamePlayerController::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void AROGamePlayerController::JumpStarted()
{
	if (ControlledHumanCharacter)
	{
		ControlledHumanCharacter->Jump();
	}
	else
	{
		ControlledHumanCharacter = Cast<AROGameCharacter>(GetPawn());
	}
}

void AROGamePlayerController::JumpStopped()
{
	if (ControlledHumanCharacter)
	{
		ControlledHumanCharacter->StopJumping();
	}
	else
	{
		ControlledHumanCharacter = Cast<AROGameCharacter>(GetPawn());
	}
}

void AROGamePlayerController::InteractObject()
{
	UE_LOG(LogROGame, Log, TEXT("Interact"));

	if (ControlledHumanCharacter)
	{
		ControlledHumanCharacter->Interact();
	}
	else
	{
		ControlledHumanCharacter = Cast<AROGameCharacter>(GetPawn());
	}
}

void AROGamePlayerController::AttackStarted()
{
	if (ControlledHumanCharacter)
	{
		if (ControlledHumanCharacter->GetEquippedWeapon())
		{
			ControlledHumanCharacter->GetEquippedWeapon()->StartAttack();
		}
		else
		{
			UE_LOG(LogROGame, Warning, TEXT("Start Attack : EquippedWeapon is null in AttackStarted"));
		}
	}
	else
	{
		UE_LOG(LogROGame, Warning, TEXT("Start Attack : ControlledHumanCharacter is null in AttackStarted"));
	}
}

void AROGamePlayerController::AttackStopped()
{
	if (ControlledHumanCharacter)
	{
		if (ControlledHumanCharacter->GetEquippedWeapon())
		{
			ControlledHumanCharacter->GetEquippedWeapon()->StopAttack();
		}
	}
}

void AROGamePlayerController::VehicleThrottle(const FInputActionValue& Value)
{
	if (ControlledVehiclePawn)
	{
		ControlledVehiclePawn->DoThrottle(Value.Get<float>());
	}
	else
	{
		ControlledVehiclePawn = Cast<AROVehiclePawnBase>(GetPawn());
	}
}

void AROGamePlayerController::VehicleSteering(const FInputActionValue& Value)
{
	if (ControlledVehiclePawn)
	{
		ControlledVehiclePawn->DoSteering(Value.Get<float>());
	}
	else
	{
		ControlledVehiclePawn = Cast<AROVehiclePawnBase>(GetPawn());
	}
}

void AROGamePlayerController::VehicleBrake(const FInputActionValue& Value)
{
	if (ControlledVehiclePawn)
	{
		ControlledVehiclePawn->DoBrake(Value.Get<float>());
	}
	else
	{
		ControlledVehiclePawn = Cast<AROVehiclePawnBase>(GetPawn());
	}
}

void AROGamePlayerController::VehicleStartBrake(const FInputActionValue& Value)
{
	if (ControlledVehiclePawn)
	{
		ControlledVehiclePawn->DoStartBrake();
	}
	else
	{
		ControlledVehiclePawn = Cast<AROVehiclePawnBase>(GetPawn());
	}
}

void AROGamePlayerController::VehicleStopBrake(const FInputActionValue& Value)
{
	if (ControlledVehiclePawn)
	{
		ControlledVehiclePawn->DoStopBrake();
	}
	else
	{
		ControlledVehiclePawn = Cast<AROVehiclePawnBase>(GetPawn());
	}
}

void AROGamePlayerController::VehicleHandbrakeStarted()
{
	if (ControlledVehiclePawn)
	{
		ControlledVehiclePawn->SetHandbrakeInput(true);
	}
}

void AROGamePlayerController::VehicleHandbrakeStopped()
{
	if (ControlledVehiclePawn)
	{
		ControlledVehiclePawn->SetHandbrakeInput(false);
	}
}

void AROGamePlayerController::VehicleToggleHeadlights()
{
	if (ControlledVehiclePawn)
	{
		// Assuming you have a function or light component in your vehicle base
		// ControlledVehiclePawn->ToggleHeadlights();
	}
}

void AROGamePlayerController::VehicleToggleCamera()
{
	if (ControlledVehiclePawn)
	{
		ControlledVehiclePawn->ToggleCamera();
	}
}

void AROGamePlayerController::VehicleLookAround(const FInputActionValue& Value)
{
	if (ControlledVehiclePawn)
	{
		ControlledVehiclePawn->LookAround(Value.Get<FVector2D>());
	}
}

void AROGamePlayerController::RequestExitVehicle()
{
	if (ControlledVehiclePawn)
	{
		ControlledVehiclePawn->ExitVehicle();
	}
}
