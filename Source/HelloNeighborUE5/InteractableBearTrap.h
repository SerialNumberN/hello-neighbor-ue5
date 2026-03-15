#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableBearTrap.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class USoundBase;
class ACharacter;

UCLASS()
class HELLONEIGHBORUE5_API AInteractableBearTrap : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInteractableBearTrap();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* TrapMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* TriggerBox;

	// Trap Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap Settings")
	bool bIsArmed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap Settings")
	float TrapDuration;

	// Audio Hook
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* SnapSound;

	// Overlap function
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Blueprint Implementable Event for visual changes (e.g. animating the trap snapping shut)
	UFUNCTION(BlueprintImplementableEvent, Category = "Trap Events")
	void OnTrapSnapped();

private:
	// Pointers to caught characters
	UPROPERTY()
	ACharacter* CaughtCharacter;

	// Original Walk Speed to restore
	float OriginalWalkSpeed;

	FTimerHandle TrapTimerHandle;

	// Helper function called when the trap duration finishes
	void ReleaseCaughtCharacter();
};
