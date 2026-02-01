// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/RangedWeapon/Projectile.h"

/* CUSTOM */
#include "../../Character/ROGameCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    RootComponent = CollisionComponent;

    // Tambahkan pengaturan kolisi secara eksplisit di constructor
    CollisionComponent->InitSphereRadius(5.0f);
    CollisionComponent->SetCollisionProfileName(TEXT("Projectile")); // Gunakan profil khusus atau BlockAll
    CollisionComponent->SetNotifyRigidBodyCollision(true); // WAJIB agar OnHit terpanggil

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovement->UpdatedComponent = CollisionComponent;
    ProjectileMovement->InitialSpeed = 2000.f;
    ProjectileMovement->MaxSpeed = 2000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;

    InitialLifeSpan = 3.0f; // Die after 3 seconds
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
    // Menghubungkan fungsi OnHit ke event tabrakan komponen
    CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (bIsFreezeProjectile)
    {
        if (OtherActor && OtherActor != GetOwner())
        {
            if (AROGameCharacter* Victim = Cast<AROGameCharacter>(OtherActor))
            {
                Victim->ApplyFreeze(FreezeAmount);
                UE_LOG(LogTemp, Log, TEXT("Freeze projectile fired with freeze power: %f"), FreezeAmount);
            }
        }  
    } 

	UE_LOG(LogTemp, Log, TEXT("Projectile hit: %s"), *OtherActor->GetName());
    Destroy();
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

