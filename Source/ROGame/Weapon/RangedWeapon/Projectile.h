// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

// Forward declarations to improve compile times
class USphereComponent;
class UProjectileMovementComponent;
class UStaticMeshComponent;

UCLASS()
class ROGAME_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();
	virtual void Tick(float DeltaTime) override;

	void SetFreezeAmount(float Amount) { FreezeAmount = Amount; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Function called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
protected:
	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<USphereComponent> CollisionComponent;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	/** Visual representation of the projectile */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	float LifeSpan = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	bool bIsFreezeProjectile = false;

private:
	float FreezeAmount = 0.f;
public:
	/** Returns CollisionComponent subobject **/
	FORCEINLINE USphereComponent* GetCollisionComponent() const { return CollisionComponent; }

	/** Returns ProjectileMovement subobject **/
	FORCEINLINE UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};
