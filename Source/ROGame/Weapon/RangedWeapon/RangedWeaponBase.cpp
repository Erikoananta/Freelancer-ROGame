// Fill out your copyright notice in the Description page of Project Settings.


#include "RangedWeaponBase.h"

/* CUSTOM */ 
#include "../../ROGame.h"
#include "../../Character/ROGameCharacter.h"
#include "../../Game/ROGamePlayerController.h"
#include "Projectile.h"

/* ENGINE */
#include "Components/SphereComponent.h"

ARangedWeaponBase::ARangedWeaponBase()
{
}

void ARangedWeaponBase::StartAttack()
{
	Super::StartAttack();

	switch (FireType)
	{
	case EFireType::Single:
		// Just call Fire once
		Fire();
		break;

	case EFireType::Automatic:
		// Loop the Fire function using a timer
		GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &ARangedWeaponBase::Fire, FireRate, true, 0.0f);
		break;

	case EFireType::Burst:
		// Implement burst logic here later
		break;

	case EFireType::Shotgun:
		// Logic for multiple pellets at once
		Fire();
		break;
	}
}

void ARangedWeaponBase::StopAttack()
{
	Super::StopAttack();

	GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);
}

void ARangedWeaponBase::Fire()
{
    if (!ProjectileClass || !OwningCharacter || !OwningController) return;

    // 1. Get Camera viewpoint for crosshair accuracy
    FVector CameraLocation;
    FRotator CameraRotation;
    OwningController->GetPlayerViewPoint(CameraLocation, CameraRotation);

    // 2. Trace from camera to find what the player is looking at
    FVector TraceEnd = CameraLocation + (CameraRotation.Vector() * 10000.0f);
    FHitResult Hit;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    QueryParams.AddIgnoredActor(OwningCharacter);

    FVector TargetPoint = GetWorld()->LineTraceSingleByChannel(Hit, CameraLocation, TraceEnd, ECC_Visibility, QueryParams)
        ? Hit.ImpactPoint : TraceEnd;

    FVector MuzzleLocation = WeaponMesh->GetSocketLocation(MuzzleSocketName);

    // 3. Routing logic based on FireType
    if (FireType == EFireType::Shotgun)
    {
        ShotgunFire(TargetPoint, MuzzleLocation);
    }
    else
    {
        // Calculate rotation from muzzle to target point
        FRotator ProjectileRotation = (TargetPoint - MuzzleLocation).Rotation();

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = OwningCharacter;

        AProjectile* NewProjectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, MuzzleLocation, ProjectileRotation, SpawnParams);
    }
}

void ARangedWeaponBase::ShotgunFire(const FVector& TargetPoint, const FVector& MuzzleLocation)
{
    TArray<AProjectile*> SpawnedPellets;
    // Loop to spawn multiple pellets
    for (int32 i = 0; i < PelletCount; i++)
    {
        // Calculate base rotation towards target point
        FRotator ProjectileRotation = (TargetPoint - MuzzleLocation).Rotation();

        // Apply random spread offset
        float RandomPitch = FMath::FRandRange(-ShotgunSpread, ShotgunSpread);
        float RandomYaw = FMath::FRandRange(-ShotgunSpread, ShotgunSpread);

        ProjectileRotation.Pitch += RandomPitch;
        ProjectileRotation.Yaw += RandomYaw;

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = OwningCharacter;

        AProjectile* NewPellet = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, MuzzleLocation, ProjectileRotation, SpawnParams);

        if (NewPellet)
        {
            // Abaikan Karakter dan Senjata agar tidak meledak di tangan
            NewPellet->GetCollisionComponent()->IgnoreActorWhenMoving(OwningCharacter, true);
            NewPellet->GetCollisionComponent()->IgnoreActorWhenMoving(this, true);

            // Abaikan pelet lain yang sudah di-spawn sebelumnya
            for (AProjectile* OtherPellet : SpawnedPellets)
            {
                NewPellet->GetCollisionComponent()->IgnoreActorWhenMoving(OtherPellet, true);
                OtherPellet->GetCollisionComponent()->IgnoreActorWhenMoving(NewPellet, true);
            }

            SpawnedPellets.Add(NewPellet);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Shotgun fired: %d pellets spawned"), PelletCount);
}
