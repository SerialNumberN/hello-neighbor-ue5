#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class AActor;
class UPrimitiveComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HELLONEIGHBORUE5_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// The maximum number of items the player can hold in their inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Settings")
	int32 MaxInventorySlots;

	// The distance in front of the player to spawn the item when dropped
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Settings")
	float DropDistance;

	// The socket name on the player's skeletal mesh to attach the active item to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Settings")
	FName HandSocketName;

	// Array holding references to the picked-up items
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<AActor*> InventoryItems;

	// The index of the currently "selected" or "active" item in the inventory array
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 ActiveItemIndex;

	// Attempts to pick up an actor and store it in the inventory
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool PickupItem(AActor* ItemToPickup);

	// Drops the currently active item from the inventory back into the world
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void DropActiveItem();

	// Cycles the active item index (e.g. forward or backward)
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void CycleInventory(bool bNextItem);

private:
	// Helper to handle the physics/collision changes when an item enters/leaves the inventory
	void SetItemActiveState(AActor* Item, bool bIsActive);
};
