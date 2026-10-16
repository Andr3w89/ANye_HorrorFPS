// Copyright Epic Games, Inc. All Rights Reserved.

#include "ANye_HorrorFPSCharacter.h"
#include "ANye_HorrorFPSProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "HealthWidget.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AANye_HorrorFPSCharacter

AANye_HorrorFPSCharacter::AANye_HorrorFPSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

}

void AANye_HorrorFPSCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//**Starts the player with full health. 
	CurrentHealth = MaxHealth;

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	//**Create the health widget for the local player if a widget class is set. 
	if (PlayerController && PlayerController->IsLocalController() && HealthWidgetClass)
	{
		HealthWidget = CreateWidget<UHealthWidget>(PlayerController, HealthWidgetClass);

		//** Display the health widget and update its health bar if created. 
		if (HealthWidget)
		{
			HealthWidget->AddToViewport();
			HealthWidget->UpdateHealth(CurrentHealth, MaxHealth);
		}
	}
}


float AANye_HorrorFPSCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//** Ignore damage if the amoun is zero or less, or the player is dead. 
	if (DamageAmount <= 0.0 || CurrentHealth <= 0.0f)
	{
		return 0.0f;
	}

	//**Let the parent class process the damage. 
	const float AcceptedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//**Save the previous health and subtract damage within the health limits. 
	const float PreviousHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth - FMath::Max(AcceptedDamage, 0.0f), 0.0f, MaxHealth);

	//**Update health bar if the widget exists. 
	if (HealthWidget)
	{
		HealthWidget->UpdateHealth(CurrentHealth, MaxHealth);
	}

	//**Calculate the amount of health lost. 
	const float ActualDamage = PreviousHealth - CurrentHealth;

	//**Quit game when the player's health reaches zero. 
	if (CurrentHealth <= 0.0f)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());

		UKismetSystemLibrary::QuitGame(this, PlayerController, EQuitPreference::Quit, false);


	}

	//** Return the amount of health lost. 
	return ActualDamage;
}

//** Restore health and return the amount restored. 
float AANye_HorrorFPSCharacter::Heal(float Amount)
{
	//** Skip healing if the amount is invalid IE dead or health full.
	if (Amount <= 0.0f || CurrentHealth <= 0.0f || CurrentHealth >= MaxHealth)
	{
		return 0.0f;
	}

	//** Save the health before healing. 
	const float PreviousHealth = CurrentHealth;

	//** Add health without exceed the max. 
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);

	//**Update health bar if the widget is valid. 
	if (IsValid(HealthWidget.Get()))
	{
		HealthWidget->UpdateHealth(CurrentHealth, MaxHealth);
	}

	//** Return the amount of health restored. 
	return CurrentHealth - PreviousHealth;

}

//////////////////////////////////////////////////////////////////////////// Input
void AANye_HorrorFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AANye_HorrorFPSCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AANye_HorrorFPSCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AANye_HorrorFPSCharacter::Move(const FInputActionValue& Value)
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

void AANye_HorrorFPSCharacter::Look(const FInputActionValue& Value)
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