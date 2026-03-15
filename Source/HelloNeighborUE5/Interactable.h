#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for any object that can be interacted with by the player (e.g. Doors, Switches, Drawers).
 */
class HELLONEIGHBORUE5_API IInteractable
{
	GENERATED_BODY()

public:
	// Called when the player presses the interact button on this object
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteract(class AActor* InteractingActor);
};
