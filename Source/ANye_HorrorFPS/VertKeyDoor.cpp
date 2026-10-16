// Fill out your copyright notice in the Description page of Project Settings.


#include "VertKeyDoor.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AVertKeyDoor::AVertKeyDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//**Create aand set the root component.
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	//**Create and attach the movable door mesh with collision
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(SceneRoot);
	DoorMesh->SetMobility(EComponentMobility::Movable);
	DoorMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

}

// Called when the game starts or when spawned
void AVertKeyDoor::BeginPlay()
{
	Super::BeginPlay();

	//**Save the door's closed positions. 
	ClosedRelativeLocation = DoorMesh->GetRelativeLocation();
	ClosedWorldLocation = DoorMesh->GetComponentLocation();

	//Convert world-up movement into the root's local coordinates.
	// This keeps the door moving vertically even if the acotr is rotated. 
	const FVector LocalOffset = SceneRoot->GetComponentTransform().InverseTransformVector(FVector(0.0f, 0.0f, OpenHeight));

	//** Calculate the door's open position
	OpenRelativeLocation = ClosedRelativeLocation + LocalOffset;

	//** Gets the player controller
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	//** Enables input and bind the E key to TryOpen
	if (PC)
	{
		EnableInput(PC);

		if (InputComponent)
		{
			FInputKeyBinding& Binding = InputComponent->BindKey(EKeys::E, IE_Pressed, this, &AVertKeyDoor::TryOpen);

			//Let other actors also receive E.
			Binding.bConsumeInput = false;
		}
	}
	
}

// Called every frame
void AVertKeyDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//**Skip movement if the door is not opening
	if (!bOpening)
	{
		return;
	}

	//**Calculate the next position using the opening speed
	const FVector NewLocation = FMath::VInterpConstantTo(DoorMesh->GetRelativeLocation(), OpenRelativeLocation, DeltaTime, OpenSpeed);

	//**Move the door to the new position.
	DoorMesh->SetRelativeLocation(NewLocation);

	//**Stop movement and mark the door as opened when it reaches the target.
	if (NewLocation.Equals(OpenRelativeLocation, 0.1f))
	{
		DoorMesh->SetRelativeLocation(OpenRelativeLocation);
		bOpening = false;
		bOpened = true;
	}

}

//**Try to open the door if player is within range and has the correct key. 
void AVertKeyDoor::TryOpen()
{

	if (bOpening || bOpened)
	{
		return;
	}

	//**grabs the players pawn. 
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	if (!PlayerPawn)
	{
		return;
	}

	//**Calculate the squared distance between the player and the closed door.
	const float DistanceSquared = FVector::DistSquared(PlayerPawn->GetActorLocation(), ClosedWorldLocation);

	if (DistanceSquared > FMath::Square(InteractionDistance))
	{
		return;
	}

	//**Create the inventory tag for the required key
	const FName InventoryTag(*FString::Printf(TEXT("Key.%s"), *RequiredKeyID.ToString()));

	//**Checks to see if player has correct key, displays message if not. 
	if (!PlayerPawn->ActorHasTag(InventoryTag))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Locked: Find matching key."));
		}

		return;
	}

	bOpening = true;
}

