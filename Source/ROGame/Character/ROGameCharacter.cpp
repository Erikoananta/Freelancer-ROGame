// Copyright Epic Games, Inc. All Rights Reserved.

#include "ROGameCharacter.h"
/* Custom */
#include "../Weapon/WeaponBase.h"
#include "../Components/InteractableComponent.h"
#include "../Components/ROAttributeComponent.h"
#include "../Types/ItemData.h"

/* Engine */
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"


AROGameCharacter::AROGameCharacter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bDoCollisionTest = true; // Aktifkan collision test
	CameraBoom->ProbeSize = 15.0f;       // Ukuran "bola" collision kamera (naikkan jika masih nembus)

	// Cegah SpringArm menabrak kapsul karakter sendiri agar tidak kedip-kedip
	CameraBoom->ProbeChannel = ECC_Camera;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	AttributeComponent = CreateDefaultSubobject<UROAttributeComponent>(TEXT("AttributeComponent"));
}

void AROGameCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void AROGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShouldTraceForInteractable)
	{
		TraceForInteractable();
	}

	// Only calculate if we are being controlled and have a weapon
	if (IsLocallyControlled() && EquippedWeapon)
	{
		CalculateAimOffset();

		if (EquippedWeapon->GetWeaponMesh()->DoesSocketExist(FName("LeftHandSocket")))
		{
			// 1. Ambil transform socket Weapon dalam World Space
			FTransform SocketTransformWS = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), RTS_World);

			// 2. Gunakan GetRelativeTransform untuk mengubah World Space ke Local (Component) Space
			// Ini menghitung posisi socket relatif terhadap Mesh karakter Anda.
			LeftHandTargetTransform = SocketTransformWS.GetRelativeTransform(GetMesh()->GetComponentTransform());
		}
	}

	// Logic for Freeze Decay
	if (!bIsFrozen && FreezeAmount > 0.0f)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();

		// Only start decaying if some time has passed since the last hit
		if (CurrentTime - LastFreezeHitTime > DecayDelay)
		{
			// Gradually reduce FreezeAmount over time
			FreezeAmount -= FreezeDecayRate * DeltaTime;

			// Make sure it doesn't go below zero
			FreezeAmount = FMath::Max(FreezeAmount, 0.0f);

			UE_LOG(LogTemp, Log, TEXT("Freeze Decaying: %f"), FreezeAmount);
		}
	}
}

void AROGameCharacter::TraceForInteractable()
{
	if (!FollowCamera) return; // Safety check

	// Get Location and Rotation directly from the Camera Component for accuracy
	FVector Location = FollowCamera->GetComponentLocation();
	FRotator Rotation = FollowCamera->GetComponentRotation();

	FVector End = Location + (Rotation.Vector() * InteractionDistance);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	// Perform line trace using the custom Interact trace channel
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Location, End, ECC_InteractTrace, Params);

	// UE_LOG(LogROGame, Log, TEXT("Trace Start: %s, End: %s"), *Location.ToString(), *End.ToString());

	// DrawDebugLine(GetWorld(), Location, End, FColor::Green, false, 1.0f, 0, 1.0f);

	if (bHit)
	{
		// Try to find the InteractableComponent on the hit actor
		UInteractableComponent* HitComponent = HitResult.GetActor()->FindComponentByClass<UInteractableComponent>();

		if (HitComponent)
		{
			// If we hit a new interactable component
			if (HitComponent != FocusedInteractable)
			{
				// Log the new focus found
				UE_LOG(LogROGame, Log, TEXT("New Focus: %s on Actor: %s"), *HitComponent->GetName(), *HitResult.GetActor()->GetName());

				// Unfocus the previous component if it exists
				if (FocusedInteractable)
				{
					FocusedInteractable->SetIsFocus(false);
				}

				// Set and notify the new component
				FocusedInteractable = HitComponent;
				FocusedInteractable->SetIsFocus(true);
			}
			return; // Exit early as we have found and handled the focus
		}
		else
		{
			// Log if we hit something that doesn't have an InteractableComponent
			// Useful to check if your collision channels are too broad
			// UE_LOG(LogROGame, Log, TEXT("Hit Actor %s but no InteractableComponent found."), *HitResult.GetActor()->GetName());
		}
	}

	// If the trace hits nothing or hits a non-interactable actor, clear existing focus
	if (FocusedInteractable)
	{
		UE_LOG(LogROGame, Log, TEXT("Focus Lost: %s"), *FocusedInteractable->GetName());

		FocusedInteractable->SetIsFocus(false);
		FocusedInteractable = nullptr;
	}
}

void AROGameCharacter::CalculateAimOffset()
{
	if (!GetController()) return;

	// ? 1. Ambil Base Aim Rotation (INI KUNCI UTAMA)
	FRotator AimRot = GetBaseAimRotation();

	// 2. Rotasi badan
	FRotator ActorRot = GetActorRotation();

	// 3. Delta rotasi (pakai API aman)
	FRotator Delta = (AimRot - ActorRot).GetNormalized();

	// 4. Simpan ke Aim Offset
	AimPitch = FMath::Clamp(Delta.Pitch, -90.f, 90.f);
	AimYaw = FMath::Clamp(Delta.Yaw, -90.f, 90.f);
}

void AROGameCharacter::Interact()
{
    if (FocusedInteractable)
    {
		// UE_LOG(LogROGame, Log, TEXT("Interacting with: %s"), *FocusedInteractable->GetName());
        // Execute the interaction logic on the component
        // This will trigger the OnInteract delegate (broadcast)
        FocusedInteractable->Interact(this);

        // Optional: Reset focus immediately after interaction if needed
        // FocusedInteractable = nullptr;
    }
	else
	{
		UE_LOG(LogROGame, Log, TEXT("No interactable focused to interact with."));
	}
}

void AROGameCharacter::EquipItem(UItemData* Data)
{
	if (!Data || !Data->ActorClassToSpawn) return;

	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;
	}

	// 1. Define Spawn Parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	// 2. Spawn the Actor (e.g., AWeaponBase) at the character's location
	AActor* NewEquippedItem = GetWorld()->SpawnActor<AActor>(
		Data->ActorClassToSpawn,
		GetActorLocation(),
		GetActorRotation(),
		SpawnParams
	);

	if (NewEquippedItem)
	{
		// 3. Attach to the specified socket on the Character's Mesh
		FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);

		NewEquippedItem->AttachToComponent(
			GetMesh(),
			AttachRules,
			Data->AttachmentSocketName // Using the dynamic socket name from Data Asset
		);

		UE_LOG(LogROGame, Log, TEXT("Successfully attached %s to socket %s"),
			*NewEquippedItem->GetName(), *Data->AttachmentSocketName.ToString());

		EquippedWeapon = Cast<AWeaponBase>(NewEquippedItem);
		if (EquippedWeapon)
		{
			CurrentHandlingWeaponMode = EquippedWeapon->GetHandlingWeaponMode();
			UE_LOG(LogROGame, Log, TEXT("Equipped weapon type: %d"), static_cast<int32>(CurrentHandlingWeaponMode));

			// --- SHOOTER MOVEMENT BEHAVIOUR ---
			bUseControllerRotationYaw = true;
			GetCharacterMovement()->bOrientRotationToMovement = false;
			GetCharacterMovement()->MaxWalkSpeed = 300.f;

			// Disable interaction tracing when a weapon is equipped
			// bShouldTraceForInteractable = false; 

			UE_LOG(LogROGame, Log, TEXT("Shooter movement enabled for %s"), *EquippedWeapon->GetName());
		}
		else
		{
			UE_LOG(LogROGame, Warning, TEXT("Equipped item is not a WeaponBase."));
		}
	}
}

void AROGameCharacter::ApplyFreeze(float Amount)
{
	if (bIsFrozen) return;

	// Update the last time we were hit
	LastFreezeHitTime = GetWorld()->GetTimeSeconds();

	FreezeAmount = FMath::Clamp(FreezeAmount + Amount, 0.0f, 1.0f);

	if (FreezeAmount >= 1.0f)
	{
		bIsFrozen = true;

		// 1. Stop all animations exactly where they are
		if (GetMesh()->GetAnimInstance())
		{
			GetMesh()->bNoSkeletonUpdate = true; // Stops skeletal updates
			GetMesh()->GetAnimInstance()->RootMotionMode = ERootMotionMode::IgnoreRootMotion;
		}

		// 2. Disable Input
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->SetIgnoreMoveInput(true);
			PC->SetIgnoreLookInput(true);
		}

		// 3. Set timer to unfreeze after some time
		GetWorldTimerManager().SetTimer(TimerHandle_Unfreeze, this, &AROGameCharacter::Unfreeze, 3.0f, false);

		UE_LOG(LogTemp, Warning, TEXT("Character %s is FROZEN!"), *GetName());
	}
}

void AROGameCharacter::Unfreeze()
{
	bIsFrozen = false;
	FreezeAmount = 0.0f;

	// 1. Resume animations
	GetMesh()->bNoSkeletonUpdate = false;

	// 2. Re-enable Input
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetIgnoreMoveInput(false);
		PC->SetIgnoreLookInput(false);
	}
}

