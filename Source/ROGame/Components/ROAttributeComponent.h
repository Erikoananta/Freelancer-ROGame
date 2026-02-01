// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ROAttributeComponent.generated.h"

// Delegasi untuk memberi tahu UI atau Actor ketika HP berubah
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, UROAttributeComponent*, AttrComp, float, Health, float, MaxHealth, const UDamageType*, DamageType);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROGAME_API UROAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UROAttributeComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Fungsi utama untuk memodifikasi HP
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyHealthChange(float Delta, const UDamageType* DamageType = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsAlive() const { return Health > 0.0f; }
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

protected:
	/** BASE Attribute : Health */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	float MaxHealth = 100.f;
	/** BASE Attribute : Health */
public:
	// Event yang bisa di-bind di C++ atau Blueprint
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChanged OnHealthChanged;

public:

};
