#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "InteractableDoor.generated.h"

class UStaticMeshComponent;
class UPhysicsConstraintComponent;

UCLASS()
class HELLONEIGHBORUE5_API AInteractableDoor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInteractableDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Implementation of the IInteractable interface
	virtual void OnInteract_Implementation(AActor* InteractingActor) override;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* DoorFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* DoorPanel;

	// The hinge connecting the panel to the frame, allowing it to swing using physics
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPhysicsConstraintComponent* DoorHinge;

	// Door Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
	bool bIsLocked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
	float MaxOpenAngle;

	// Force applied when the door is "slammed" or pushed hard
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Settings")
	float SlamForce;

	// Blueprint Callable function to lock/unlock the door dynamically
	UFUNCTION(BlueprintCallable, Category = "Door Settings")
	void SetDoorLocked(bool bNewLockedState);

	// Pushes the door open with physics force
	UFUNCTION(BlueprintCallable, Category = "Door Settings")
	void PushDoor(FVector PushDirection, float ForceMagnitude);

private:
	// Helper to set up the constraint limits based on locked state
	void UpdateConstraintLimits();
};
