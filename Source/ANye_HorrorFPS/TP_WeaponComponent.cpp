// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "ANye_HorrorFPSCharacter.h"
#include "ANye_HorrorFPSProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "PerlinProcTerrain.h"
#include "CollisionQueryParams.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}


void UTP_WeaponComponent::Fire()
{
	//**Checks to ensure the weapon has a valid character. 
	if (Character == nullptr)
	{
		return;
	}

	//**Gets player controller to read camera position and aiming direction
	APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());

	//**Grabs the current game world to perform the raycast and spawn projectile
	UWorld* World = GetWorld();

	//**Ensures playercontroller and world are available. 
	if (PlayerController == nullptr || World == nullptr)
	{
		return;
	}

	//Get player's camera position and aiming direction.
	FVector ViewLocation;
	FRotator ViewRotation;
	PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

	//**Conduct line trace of 10,000 units in the direction the player is looking. 
	const FVector TraceEnd = ViewLocation + ViewRotation.Vector() * 10000.0f;

	//**Hit stores the information returned by the raycast and specifies collision-query.
	FHitResult Hit;
	FCollisionQueryParams TraceParams;

	//**Ignore the player and actor that owns the weapon
	TraceParams.AddIgnoredActor(Character);
	if (GetOwner() != nullptr)
	{
		TraceParams.AddIgnoredActor(GetOwner());
	}

	TraceParams.bTraceComplex = true;

	//**Alter terrain at the first blocking hit, if it is terrain.
	if (World->LineTraceSingleByChannel(Hit, ViewLocation, TraceEnd, ECC_Visibility, TraceParams))
	{
		APerlinProcTerrain* Terrain = Cast<APerlinProcTerrain>(Hit.GetActor());

		if (Terrain != nullptr)
		{
			Terrain->AlterMesh(Hit.ImpactPoint);
		}
	}

	//Spawn the projectile.
	if (ProjectileClass != nullptr && GetOwner() != nullptr)
	{
		const FVector SpawnLocation = GetOwner()->GetActorLocation() + ViewRotation.RotateVector(MuzzleOffset);

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		World->SpawnActor<AANye_HorrorFPSProjectile>(ProjectileClass, SpawnLocation, ViewRotation, ActorSpawnParams);
	}

	//Play firing sound.
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}

	//Play firing animation.
	if (FireAnimation != nullptr)
	{
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();

		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}


bool UTP_WeaponComponent::AttachWeapon(AANye_HorrorFPSCharacter* TargetCharacter)
{
	Character = TargetCharacter;

	// Check that the character is valid, and has no weapon component yet
	if (Character == nullptr || Character->GetInstanceComponents().FindItemByClass<UTP_WeaponComponent>())
	{
		return false;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	// add the weapon as an instance component to the character
	Character->AddInstanceComponent(this);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);
		}
	}

	return true;
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}