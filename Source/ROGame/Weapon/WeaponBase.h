// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../ROGame.h"
#include "WeaponBase.generated.h"

class USkeletalMeshComponent;
class UStaticMeshComponent;
class AROGameCharacter;
class AROGamePlayerController;

UCLASS()
class ROGAME_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponBase();
	virtual void Tick(float DeltaTime) override;

    // WeaponBase.h
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void StartAttack();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void StopAttack();
protected:
	virtual void BeginPlay() override;

protected:
    // Komponen utama (SceneRoot)
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<USceneComponent> SceneRoot;

	// Default Weapon Mesh (Skeletal Mesh)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USkeletalMeshComponent> WeaponMesh;

    // For Static Mesh (Optional, if you want to use Static Mesh instead Skeletal Mesh)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> WeaponStaticMesh;

    /** The animation type for this specific weapon */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
    EHandlingWeaponMode HandlingWeaponMode = EHandlingWeaponMode::None;

	// Reference to the owning character
    UPROPERTY()
    TObjectPtr<AROGameCharacter> OwningCharacter;

    // Reference to the Owning Character Controller
    UPROPERTY()
	TObjectPtr<AROGamePlayerController> OwningController;
private:
    
public:	
	FORCEINLINE EHandlingWeaponMode GetHandlingWeaponMode() const { return HandlingWeaponMode; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
};
