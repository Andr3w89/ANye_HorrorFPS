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
#include "PerlinRockDeform.h"
#include "TimerManager.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "CollisionQueryParams.h"
#include "GameFramework/Pawn.h"

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

	//**Trace the player's aim and store whether something was hit. 
	const bool bTraceHit = World->LineTraceSingleByChannel(Hit, ViewLocation, TraceEnd, ECC_Visibility, TraceParams);

	//** Change the terrain mesh if the trace hits terrain. 
	if (bTraceHit)
	{
		if (APerlinProcTerrain* Terrain = Cast<APerlinProcTerrain>(Hit.GetActor()))
		{
			Terrain->AlterMesh(Hit.ImpactPoint);
		}
	}


	//Spawn the projectile.
	if (ProjectileClass != nullptr)
	{
		//** Calculate the projectiles spawn location using the muzzle offset. 
		const FVector SpawnLocation = Character->GetActorLocation() + ViewRotation.RotateVector(MuzzleOffset);

		//**Aim the projectile toward the hit point or the end of the trace. 
		const FVector AimPoint = bTraceHit ? Hit.ImpactPoint : TraceEnd;
		const FRotator ProjectileRotation = (AimPoint - SpawnLocation).Rotation();

		//** Set the shooter and adjust the spawn location if blocked. 
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.Owner = Character;
		ActorSpawnParams.Instigator = Character;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		
		//**Spawn the projectile at the calculaated location and rotation. 
		AANye_HorrorFPSProjectile* Projectile = World->SpawnActor<AANye_HorrorFPSProjectile>(ProjectileClass, SpawnLocation, ProjectileRotation, ActorSpawnParams);

		//** check that the projectile spawned and the trace hit something. 
		if (Projectile && bTraceHit)
		{
			//**Check if the trace hit a rock. 
			APerlinRockDeform* Rock = Cast<APerlinRockDeform>(Hit.GetActor());
			if (Rock)
			{
				const UProjectileMovementComponent* Movement = Projectile->FindComponentByClass<UProjectileMovementComponent>();
				const float ProjectileSpeed = Movement ? Movement->Velocity.Size() : 0.0f;

				//** Estimate the travel time if the projectile is moving. 
				if (ProjectileSpeed > KINDA_SMALL_NUMBER)
				{
					const float TravelDistance = FVector::Distance(Projectile->GetActorLocation(), Hit.ImpactPoint);
					const float TravelTime = TravelDistance / ProjectileSpeed;

					//**Save a weak rock reference and the impact details for the delayed hit. 
					const TWeakObjectPtr<APerlinRockDeform> WeakRock(Rock);
					const FVector ImpactPoint = Hit.ImpactPoint;
					const FVector ImpactNormal = Hit.ImpactNormal;

					//**Create the action to run after the travel delay. 
					FTimerDelegate DelayedHit;
					DelayedHit.BindLambda([WeakRock, ImpactPoint, ImpactNormal]()
						{
							//**Deal damage and dent the rock if it still exists. 
							if (APerlinRockDeform* TargetRock = WeakRock.Get())
							{
								TargetRock->HitRock(ImpactPoint, ImpactNormal, 25.0f);
							}
						});

					//**Run the delayed hit once after the estimated travel time. 
					FTimerHandle ShotTimer;
					World->GetTimerManager().SetTimer(ShotTimer, DelayedHit, FMath::Max(TravelTime, 0.001f), false);
				}
				//** log a warning if the projectile is not moving. 
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Rock hit not scheduled: projectile needs a moving ProjectileMovementComponent"));
				}
			}
		}
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