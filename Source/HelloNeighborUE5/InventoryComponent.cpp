#include "InventoryComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // Doesn't need to tick

	MaxInventorySlots = 4; // Default like Hello Neighbor
	DropDistance = 150.0f;
	HandSocketName = TEXT("hand_r"); // Default standard UE socket name
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

	// Ensure we only have one item active
	CycleInventory(true); // Re-run cycle logic to update visuals

	return true;
}

void UInventoryComponent::DropActiveItem()
{
	if (InventoryItems.IsEmpty() || !InventoryItems.IsValidIndex(ActiveItemIndex))
	{
		return;
	}

	AActor* ItemToDrop = InventoryItems[ActiveItemIndex];

	// Remove it from the inventory array FIRST to prevent infinite loops elsewhere
	// even if the actor itself became invalid while in our pocket
	InventoryItems.RemoveAt(ActiveItemIndex);

	if (!ItemToDrop) return;

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

	CycleInventory(true); // Update visuals for the next item
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

	// Update visuals
	for (int32 i = 0; i < InventoryItems.Num(); ++i)
	{
		AActor* Item = InventoryItems[i];
		if (!Item) continue;

		if (i == ActiveItemIndex)
		{
			// Make active item visible and attach to hand
			Item->SetActorHiddenInGame(false);

			ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
			if (OwnerCharacter && OwnerCharacter->GetMesh())
			{
				Item->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, HandSocketName);
			}
			else
			{
				Item->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepRelativeTransform);
			}
		}
		else
		{
			// Hide inactive items
			Item->SetActorHiddenInGame(true);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Active Inventory Slot: %d"), ActiveItemIndex);
}
