#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableWindow.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class HELLONEIGHBORUE5_API AInteractableWindow : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInteractableWindow();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// The glass pane
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* WindowGlass;

	// Collision box to detect thrown objects hitting the window
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BreakCollision;

	// The threshold of force required to break the window
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Window Settings")
	float BreakThreshold;

	// Whether the window has already been shattered
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Window State")
	bool bIsBroken;

	// Function to call when something hits the collision box
	UFUNCTION()
	void OnWindowHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Sounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* ShatterSound;

	// Blueprint Implementable Event to handle visual effects (particles, sound, replacing mesh with broken pieces)
	UFUNCTION(BlueprintImplementableEvent, Category = "Window Events")
	void OnWindowShatter();
};
