#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HelloNeighborGameMode.generated.h"

/**
 * The core game mode for Hello Neighbor handling high-level logic and resets.
 */
UCLASS()
class HELLONEIGHBORUE5_API AHelloNeighborGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHelloNeighborGameMode();

	// Function to call when the player is caught by the Neighbor
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void OnPlayerCaught();

	// The time (in seconds) before the level fully resets after being caught
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Flow")
	float RespawnDelay;

	// Reset logic exposed to Blueprints for UI hooks
	UFUNCTION(BlueprintNativeEvent, Category = "Game Flow")
	void ResetLevel();

protected:
	FTimerHandle RespawnTimerHandle;
};
