// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/RangedWeapon/RangedWeaponBase.h"
#include "FreezeRangeWeapon.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
/**
 * 
 */
UCLASS()
class ROGAME_API AFreezeRangeWeapon : public ARangedWeaponBase
{
	GENERATED_BODY()

public:
	AFreezeRangeWeapon();

	// Start firing projectiles
	virtual void StartAttack() override;
	// Stop firing projectiles
	virtual void StopAttack() override;

protected:
	virtual void BeginPlay() override;
	virtual void Fire() override;

protected:
	/** Amount of freeze applied to the target per hit (0.0 to 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	float FreezePower = 0.05f;

	/** Specific fire rate for freeze mode (usually very fast for spray feel) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	float FreezeFireRate = 0.05f;

	/** Jarak maksimal semprotan es (2 meter = 200 unit) */
	UPROPERTY(EditAnywhere, Category = "Setup")
	float MaxFreezeRange = 200.0f;

	/** Radius area semprotan */
	UPROPERTY(EditAnywhere, Category = "Setup")
	float FreezeSphereRadius = 40.0f;

	/** VFX untuk semprotan es */
	UPROPERTY(EditAnywhere, Category = "Setup")
	TObjectPtr<UNiagaraSystem> IceSprayVFX;

	/** Komponen untuk mengontrol spawn VFX secara terus-menerus */
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ActiveVFXComponent;
};
