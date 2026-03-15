#include "InventoryComponent.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // Doesn't need to tick

	MaxInventorySlots = 4; // Default like Hello Neighbor
	DropDistance = 150.0f;
	ActiveItemIndex = 0;
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInventoryComponent::SetItemActiveState(AActor* Item, bool bIsActive)
{
	if (!Item) return;

	// Hide or Show the actor
	Item->SetActorHiddenInGame(!bIsActive);

	// Find the root component to toggle physics and collision
	UPrimitiveComponent* RootPrimComp = Cast<UPrimitiveComponent>(Item->GetRootComponent());
	if (RootPrimComp)
	{
		RootPrimComp->SetSimulatePhysics(bIsActive);

		if (bIsActive)
		{
			// Re-enable collision
			RootPrimComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
		else
		{
			// Disable collision entirely while in the pocket
			RootPrimComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

bool UInventoryComponent::PickupItem(AActor* ItemToPickup)
{
	if (!ItemToPickup) return false;

	// Check if inventory is full
	if (InventoryItems.Num() >= MaxInventorySlots)
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory is full!"));
		return false;
	}

	// Add to array
	InventoryItems.Add(ItemToPickup);

	// Deactivate it (hide it, stop physics, stop collision)
	SetItemActiveState(ItemToPickup, false);

	// Optionally, attach it to the player so it moves with them (hidden in pocket)
	ItemToPickup->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepRelativeTransform);

	return true;
}

void UInventoryComponent::DropActiveItem()
{
	if (InventoryItems.IsEmpty() || !InventoryItems.IsValidIndex(ActiveItemIndex))
	{
		return;
	}

	AActor* ItemToDrop = InventoryItems[ActiveItemIndex];
	if (!ItemToDrop) return;

	// Remove it from the inventory array
	InventoryItems.RemoveAt(ActiveItemIndex);

	// Detach it from the player
	ItemToDrop->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// Calculate drop location (in front of the player's camera)
	UCameraComponent* PlayerCamera = GetOwner()->FindComponentByClass<UCameraComponent>();
	if (PlayerCamera)
	{
		FVector DropLocation = PlayerCamera->GetComponentLocation() + (PlayerCamera->GetForwardVector() * DropDistance);
		ItemToDrop->SetActorLocation(DropLocation);
	}
	else
	{
		// Fallback if no camera
		FVector DropLocation = GetOwner()->GetActorLocation() + (GetOwner()->GetActorForwardVector() * DropDistance);
		ItemToDrop->SetActorLocation(DropLocation);
	}

	// Reactivate it (show it, start physics, enable collision)
	SetItemActiveState(ItemToDrop, true);

	// Adjust ActiveItemIndex if we dropped the last item in the array
	if (InventoryItems.Num() > 0 && ActiveItemIndex >= InventoryItems.Num())
	{
		ActiveItemIndex = InventoryItems.Num() - 1;
	}
}

void UInventoryComponent::CycleInventory(bool bNextItem)
{
	if (InventoryItems.IsEmpty()) return;

	if (bNextItem)
	{
		ActiveItemIndex++;
		if (ActiveItemIndex >= InventoryItems.Num())
		{
			ActiveItemIndex = 0; // Wrap around to the beginning
		}
	}
	else
	{
		ActiveItemIndex--;
		if (ActiveItemIndex < 0)
		{
			ActiveItemIndex = InventoryItems.Num() - 1; // Wrap around to the end
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Active Inventory Slot: %d"), ActiveItemIndex);
}
