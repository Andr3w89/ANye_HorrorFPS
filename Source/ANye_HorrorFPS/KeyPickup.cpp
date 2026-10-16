// Fill out your copyright notice in the Description page of Project Settings.


#include "KeyPickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"

// Sets default values
AKeyPickup::AKeyPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//**Create the pickup sphere and set it as the root component. 
	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	SetRootComponent(PickupSphere);

	//**Set the spheres size and enable pawn overlap detection
	PickupSphere->InitSphereRadius(80.0f);
	PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	PickupSphere->SetGenerateOverlapEvents(true);

	//**Create and attach the key mesh with collision disabled. 
	KeyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KeyMesh"));
	KeyMesh->SetupAttachment(PickupSphere);
	KeyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void AKeyPickup::BeginPlay()
{
	Super::BeginPlay();
	//**Call onpickupoverlap when something overlaps the sphere
	PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &AKeyPickup::OnPickupOverlap);
	
}

void AKeyPickup::OnPickupOverlap(UPrimitiveComponent* OverlaappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	//**Check whether the overlapping actor is a pawn
	APawn* PlayerPawn = Cast<APawn>(OtherActor);

	//**Stop if collected, the pawn is not a player, or the key ID is missing. 
	if (bCollected || !PlayerPawn || !PlayerPawn->IsPlayerControlled() || KeyID.IsNone())
	{
		return;
	}

	//Prefix the ID to distinguish inventory keys from other actor taags.
	const FName InventoryTag(*FString::Printf(TEXT("Key.%s"), *KeyID.ToString()));

	//** Add the key tag to the player without duplicates. 
	PlayerPawn->Tags.AddUnique(InventoryTag);

	//** Mark the key as collected and remove the pickup
	bCollected = true;
	Destroy();
}

// Called every frame
void AKeyPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

