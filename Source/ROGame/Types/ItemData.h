// ItemData.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemData.generated.h"

/** Enum to categorize items */
UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon,
	Consumable,
	Vehicle,
	Generic
};

UCLASS(BlueprintType)
class ROGAME_API UItemData : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Display name of the item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "General")
	FString ItemName;

	/** Type of the item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "General")
	EItemType ItemType;

	/** Mesh for the pickup actor in the world */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<USkeletalMesh> PickupSkeletalMesh;

	/** Mesh for the pickup actor in the world */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<UStaticMesh> PickupStaticMesh;

	/** The Actor class to spawn when this item is equipped/used */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	TSubclassOf<AActor> ActorClassToSpawn;

	/** Icon for the Inventory UI */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<UTexture2D> ItemIcon;

	/** The skeletal mesh socket name where this item should attach when equipped */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	FName AttachmentSocketName = FName("RangedWeaponSocket"); // Default value
};
