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

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(SceneRoot);
	DoorMesh->SetMobility(EComponentMobility::Movable);
	DoorMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

}

// Called when the game starts or when spawned
void AVertKeyDoor::BeginPlay()
{
	Super::BeginPlay();

	ClosedRelativeLocation = DoorMesh->GetRelativeLocation();
	ClosedWorldLocation = DoorMesh->GetComponentLocation();

	//Convert world-up movement into the root's local coordinates.
	// This keeps the door moving vertically even if the acotr is rotated. 
	const FVector LocalOffset = SceneRoot->GetComponentTransform().InverseTransformVector(FVector(0.0f, 0.0f, OpenHeight));

	OpenRelativeLocation = ClosedRelativeLocation + LocalOffset;

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

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

	if (!bOpening)
	{
		return;
	}

	const FVector NewLocation = FMath::VInterpConstantTo(DoorMesh->GetRelativeLocation(), OpenRelativeLocation, DeltaTime, OpenSpeed);

	DoorMesh->SetRelativeLocation(NewLocation);

	if (NewLocation.Equals(OpenRelativeLocation, 0.1f))
	{
		DoorMesh->SetRelativeLocation(OpenRelativeLocation);
		bOpening = false;
		bOpened = true;
	}

}

void AVertKeyDoor::TryOpen()
{

	if (bOpening || bOpened)
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	if (!PlayerPawn)
	{
		return;
	}

	const float DistanceSquared = FVector::DistSquared(PlayerPawn->GetActorLocation(), ClosedWorldLocation);

	if (DistanceSquared > FMath::Square(InteractionDistance))
	{
		return;
	}

	const FName InventoryTag(*FString::Printf(TEXT("Key.%s"), *RequiredKeyID.ToString()));

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

