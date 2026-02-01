// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../WeaponBase.h"
#include "RangedWeaponBase.generated.h"


// Define weapon types for animation and logic
UENUM(BlueprintType)
enum class EFireType : uint8
{
    Single      UMETA(DisplayName = "Single"),
    Automatic   UMETA(DisplayName = "Automatic"),
    Burst       UMETA(DisplayName = "Burst"),
    Shotgun     UMETA(DisplayName = "Shotgun"),
	Freeze	    UMETA(DisplayName = "Freeze Spray")
};

/**
 * 
 */
UCLASS()
class ROGAME_API ARangedWeaponBase : public AWeaponBase
{
	GENERATED_BODY()
public:
	ARangedWeaponBase();
	
    // Start firing projectiles
    virtual void StartAttack() override;
    // Stop firing projectiles
    virtual void StopAttack() override;
protected:
    UPROPERTY(EditAnywhere, Category = "Setup")
    TSubclassOf<class AProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
    EFireType FireType = EFireType::Single;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	FName MuzzleSocketName = "MuzzleSocket";

    /** For Automatic: Time between shots. For Burst: Delay between bursts. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
    float FireRate = 0.15f;

    /** Number of projectiles per shot (Only for Shotgun) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
    int32 PelletCount = 8;

    /** Spread angle in degrees (Only for Shotgun) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
    float ShotgunSpread = 3.5f;

    // Timer for handling automatic/burst logic
    FTimerHandle TimerHandle_HandleFiring;
private:

protected:
	// Fire a single projectile
    virtual void Fire();


    /** Specialized function for shotgun pellet logic */
    void ShotgunFire(const FVector& TargetPoint, const FVector& MuzzleLocation);
};
