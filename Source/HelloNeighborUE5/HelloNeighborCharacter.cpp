#include "HelloNeighborCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/LocalPlayer.h"
#include "InteractionComponent.h"
#include "InventoryComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogTemplateCharacter, Log, All);

AHelloNeighborCharacter::AHelloNeighborCharacter()
{
	PrimaryActorTick.bCanEverTick = true; // Ensure character ticks

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Note: InteractionComponent internally requires a PhysicsHandle attached to the actor to work
	UPhysicsHandleComponent* PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	WalkSpeed = 400.0f;
	SprintSpeed = 800.0f;

	MaxStamina = 100.0f;
	CurrentStamina = MaxStamina;
	StaminaDrainRate = 15.0f;
	StaminaRegenRate = 10.0f;
	bIsSprinting = false;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

void AHelloNeighborCharacter::BeginPlay()
{
	// Call the base class
	Super::BeginPlay();

	// Ensure Blueprint-modified WalkSpeed takes effect at runtime
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

}

void AHelloNeighborCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHelloNeighborCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AHelloNeighborCharacter::Look);

		// Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AHelloNeighborCharacter::StartCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AHelloNeighborCharacter::StopCrouch);

		// Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AHelloNeighborCharacter::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AHelloNeighborCharacter::StopSprint);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AHelloNeighborCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AHelloNeighborCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AHelloNeighborCharacter::StartCrouch()
{
	Crouch();
}

void AHelloNeighborCharacter::StopCrouch()
{
	UnCrouch();
}

void AHelloNeighborCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle Stamina Drain and Regeneration
	bool bStaminaChanged = false;

	if (bIsSprinting)
	{
		// Only drain stamina if actually moving (velocity > ~10 units/sec) to avoid physics jitter draining stamina while standing still
		if (GetVelocity().SizeSquared() > 100.0f)
		{
			CurrentStamina -= StaminaDrainRate * DeltaTime;
			bStaminaChanged = true;

			if (CurrentStamina <= 0.0f)
			{
				CurrentStamina = 0.0f;
				StopSprint(); // Force stop sprinting when out of stamina
			}
		}
	}
	else
	{
		// Regenerate stamina when not sprinting
		if (CurrentStamina < MaxStamina)
		{
			CurrentStamina += StaminaRegenRate * DeltaTime;
			bStaminaChanged = true;

			if (CurrentStamina > MaxStamina)
			{
				CurrentStamina = MaxStamina;
			}
		}
	}

	// Trigger Blueprint HUD event if it changed
	if (bStaminaChanged)
	{
		OnStaminaChanged(CurrentStamina, MaxStamina);
	}
}

void AHelloNeighborCharacter::StartSprint()
{
	if (CurrentStamina > 0.0f && !bIsCrouched)
	{
		bIsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void AHelloNeighborCharacter::StopSprint()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}
