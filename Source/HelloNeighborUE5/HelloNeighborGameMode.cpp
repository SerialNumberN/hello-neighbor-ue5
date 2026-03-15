#include "HelloNeighborGameMode.h"
#include "HelloNeighborCharacter.h"
#include "InteractableBearTrap.h"
#include "InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "TimerManager.h"

#include "NeighborAIController.h"
#include "UObject/ConstructorHelpers.h"

AHelloNeighborGameMode::AHelloNeighborGameMode()
{
	RespawnDelay = 3.0f; // Default 3 second respawn wait

	// Register default classes (typically overridden in Editor, but good practice to set base C++ defaults)
	DefaultPawnClass = AHelloNeighborCharacter::StaticClass();
	// AI Controllers are usually assigned directly on the AI Character Blueprint's "AI Controller Class" setting
	// rather than in the GameMode, but we ensure the classes are linked.
}

void AHelloNeighborGameMode::OnPlayerCaught()
{
	UE_LOG(LogTemp, Warning, TEXT("GameMode: Player Caught! Preparing reset..."));

	// Set a timer to reset the level state after the RespawnDelay finishes
	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &AHelloNeighborGameMode::ResetLevel, RespawnDelay, false);
}

void AHelloNeighborGameMode::ResetLevel_Implementation()
{
	// 1. Reset the Player Character
	AHelloNeighborCharacter* PlayerCharacter = Cast<AHelloNeighborCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (PlayerCharacter)
	{
		// Restore stamina
		PlayerCharacter->CurrentStamina = PlayerCharacter->MaxStamina;

		// Drop all inventory items
		UInventoryComponent* InventoryComp = PlayerCharacter->FindComponentByClass<UInventoryComponent>();
		if (InventoryComp)
		{
			// Safely loop through and drop all items without risking an infinite loop
			// if an item became null/invalid while in the inventory array.
			int32 NumItems = InventoryComp->InventoryItems.Num();
			for (int32 i = NumItems - 1; i >= 0; --i)
			{
				InventoryComp->ActiveItemIndex = i;
				InventoryComp->DropActiveItem();
			}
		}

		// (Optional) Teleport player back to start location
		// PlayerCharacter->SetActorLocation(StartTransform);
	}

	// 2. Reset all Bear Traps in the level
	for (TActorIterator<AInteractableBearTrap> It(GetWorld()); It; ++It)
	{
		AInteractableBearTrap* Trap = *It;
		if (Trap)
		{
			Trap->bIsArmed = true; // Rearm the trap
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("GameMode: Level state reset."));
}
