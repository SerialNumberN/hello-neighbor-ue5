#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NeighborAICharacter.generated.h"

// Enum to define the core AI States for the Behavior Tree and logic
UENUM(BlueprintType)
enum class ENeighborState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Patrolling UMETA(DisplayName = "Patrolling"),
	Investigating UMETA(DisplayName = "Investigating"),
	Chasing UMETA(DisplayName = "Chasing"),
	Searching UMETA(DisplayName = "Searching")
};

UCLASS(config=Game)
class HELLONEIGHBORUE5_API ANeighborAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANeighborAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Current State of the Neighbor (drives the Behavior Tree)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI State")
	ENeighborState CurrentState;

	// Sounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* FootstepSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* AlertSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* PatrolSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* InvestigateSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
	float FootstepInterval;

private:
	float LastFootstepTime;

public:
	// Function to change state and notify the AI Controller
	UFUNCTION(BlueprintCallable, Category = "AI State")
	void SetNeighborState(ENeighborState NewState);

	// The Catch Mechanic
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Catch Mechanic")
	class USphereComponent* CatchRadius;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Blueprint Implementable Event to handle the Game Over screen or reset logic
	UFUNCTION(BlueprintImplementableEvent, Category = "Game Events")
	void OnPlayerCaught();
};
