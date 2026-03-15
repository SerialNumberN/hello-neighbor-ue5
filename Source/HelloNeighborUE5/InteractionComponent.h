#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class UPhysicsHandleComponent;
class UCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HELLONEIGHBORUE5_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Perform a raycast to interact or grab an object
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Interact();

	// Release currently held object or throw it
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Release(bool bThrow);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Settings")
	float ReachDistance = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Settings")
	float ThrowForce = 1500.0f;

	// Distance to hold the grabbed object in front of the camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Settings")
	float HoldDistance = 150.0f;

private:
	// Component used to hold physics objects
	UPhysicsHandleComponent* PhysicsHandle;

	// Reference to the player's camera to raycast from
	UCameraComponent* PlayerCamera;

	bool bIsHoldingObject;

	// Internal helper to get the line trace start and end points
	void GetTraceData(FVector& OutTraceStart, FVector& OutTraceEnd) const;

};
