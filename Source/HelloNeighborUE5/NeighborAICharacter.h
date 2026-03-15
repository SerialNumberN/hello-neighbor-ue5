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

	// Component to hold AI Senses (Sight, Hearing, etc.)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class UAIPerceptionComponent* AIPerceptionComponent;

	// Sight Config
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class UAISenseConfig_Sight* SightConfig;

	// Hearing Config
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class UAISenseConfig_Hearing* HearingConfig;

	// Current State of the Neighbor (drives the Behavior Tree)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI State")
	ENeighborState CurrentState;

	// Function to be called when the perception system detects a new stimulus
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	// Sounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* FootstepSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* AlertSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Settings")
	float FootstepInterval;

private:
	float LastFootstepTime;

public:
	// Function to change state and notify the AI Controller
	UFUNCTION(BlueprintCallable, Category = "AI State")
	void SetNeighborState(ENeighborState NewState);

	// The Catch Mechanic
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Blueprint Implementable Event to handle the Game Over screen or reset logic
	UFUNCTION(BlueprintImplementableEvent, Category = "Game Events")
	void OnPlayerCaught();
};
