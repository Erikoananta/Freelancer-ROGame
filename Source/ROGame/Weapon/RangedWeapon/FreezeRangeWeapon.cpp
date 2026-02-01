// Fill out your copyright notice in the Description page of Project Settings.


#include "FreezeRangeWeapon.h"

/* CUSTOM */
#include "../../Character/ROGameCharacter.h"
#include "../../Game/ROGamePlayerController.h"

/* ENGINE */
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AFreezeRangeWeapon::AFreezeRangeWeapon()
{
	FireType = EFireType::Freeze;
	FreezeFireRate = 0.05f; // Sangat cepat agar terasa mulus seperti spray
}

void AFreezeRangeWeapon::StartAttack()
{
	Super::StartAttack();

	// High-frequency firing to simulate a spray/beam
	GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AFreezeRangeWeapon::Fire, FreezeFireRate, true, 0.0f);

	// Activate VFX Spray for ice effect
	if (IceSprayVFX && !ActiveVFXComponent)
	{
		ActiveVFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			IceSprayVFX, WeaponMesh, MuzzleSocketName,
			FVector::ZeroVector, FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget, true
		);
	}

}

void AFreezeRangeWeapon::StopAttack()
{
	Super::StopAttack();

	GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);

	// Disable VFX Spray
	if (ActiveVFXComponent)
	{
		ActiveVFXComponent->Deactivate();
		ActiveVFXComponent = nullptr;
	}
}

void AFreezeRangeWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AFreezeRangeWeapon::Fire()
{
	// Kita abaikan logika spawn proyektil milik base class untuk optimasi
	if (!OwningCharacter || !OwningController) return;

	FVector MuzzleLocation = WeaponMesh->GetSocketLocation(MuzzleSocketName);

	// Arah tembakan berdasarkan rotasi kamera agar akurat ke crosshair
	FVector CameraLocation;
	FRotator CameraRotation;
	OwningController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector TraceStart = MuzzleLocation;
	FVector TraceEnd = TraceStart + (CameraRotation.Vector() * MaxFreezeRange);

	TArray<FHitResult> HitResults;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(OwningCharacter);

	// Sphere Trace Multi: Mendeteksi semua target dalam volume semprotan
	bool bHit = UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(), TraceStart, TraceEnd, FreezeSphereRadius,
		UEngineTypes::ConvertToTraceType(ECC_Pawn), false,
		ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResults, true
	);
	

	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			if (AROGameCharacter* Victim = Cast<AROGameCharacter>(Hit.GetActor()))
			{
				// Berikan efek beku per hit/tick
				Victim->ApplyFreeze(FreezePower);

				UE_LOG(LogTemp, Log, TEXT("Freezing target: %s with power: %f"), *Victim->GetName(), FreezePower);
			}
		}
	}
}
