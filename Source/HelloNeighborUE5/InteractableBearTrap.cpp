#include "InteractableBearTrap.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HelloNeighborCharacter.h"
#include "NeighborAICharacter.h"
#include "NeighborAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

// Sets default values
AInteractableBearTrap::AInteractableBearTrap()
{
	PrimaryActorTick.bCanEverTick = false;

	// Setup visual mesh
	TrapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrapMesh"));
	RootComponent = TrapMesh;

	// Setup trigger box
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetBoxExtent(FVector(30.0f, 30.0f, 10.0f));
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));

	// Defaults
	bIsArmed = true;
	TrapDuration = 3.0f;
	CaughtCharacter = nullptr;
	OriginalWalkSpeed = 0.0f;
}

// Called when the game starts or when spawned
void AInteractableBearTrap::BeginPlay()
{
	Super::BeginPlay();

	// Bind overlap
	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AInteractableBearTrap::OnOverlapBegin);
	}
}

void AInteractableBearTrap::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsArmed || !OtherActor || OtherActor == this) return;

	// Check if a character stepped on it
	ACharacter* HitCharacter = Cast<ACharacter>(OtherActor);
	if (HitCharacter)
	{
		// Snap the trap
		bIsArmed = false;
		CaughtCharacter = HitCharacter;
		OriginalWalkSpeed = CaughtCharacter->GetCharacterMovement()->MaxWalkSpeed;

		// Play sound
		if (SnapSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, SnapSound, GetActorLocation());
		}

		// Trigger Blueprint event for visuals
		OnTrapSnapped();

		// Handle Player vs Neighbor differently
		if (AHelloNeighborCharacter* PlayerCharacter = Cast<AHelloNeighborCharacter>(CaughtCharacter))
		{
			// Drastically reduce speed for player (or set to 0 to completely trap them)
			PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = 0.0f;
		}
		else if (ANeighborAICharacter* NeighborCharacter = Cast<ANeighborAICharacter>(CaughtCharacter))
		{
			// Pause the Neighbor's movement
			NeighborCharacter->GetCharacterMovement()->MaxWalkSpeed = 0.0f;

			// Notify the AI Controller to set the Blackboard key so the Behavior Tree transitions cleanly
			ANeighborAIController* AIController = Cast<ANeighborAIController>(NeighborCharacter->GetController());
			if (AIController)
			{
				AIController->SetTrappedState(true);
			}
		}

		// Set a timer to release them
		GetWorld()->GetTimerManager().SetTimer(TrapTimerHandle, this, &AInteractableBearTrap::ReleaseCaughtCharacter, TrapDuration, false);
	}
}

void AInteractableBearTrap::ReleaseCaughtCharacter()
{
	if (CaughtCharacter)
	{
		// Restore movement
		CaughtCharacter->GetCharacterMovement()->MaxWalkSpeed = OriginalWalkSpeed;

		// If it was the Neighbor, resume his Behavior Tree
		if (ANeighborAICharacter* NeighborCharacter = Cast<ANeighborAICharacter>(CaughtCharacter))
		{
			ANeighborAIController* AIController = Cast<ANeighborAIController>(NeighborCharacter->GetController());
			if (AIController)
			{
				AIController->SetTrappedState(false);
			}
		}

		// Clear reference
		CaughtCharacter = nullptr;
	}
}
