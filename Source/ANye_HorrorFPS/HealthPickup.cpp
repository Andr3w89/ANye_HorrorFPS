// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"
#include "ANye_HorrorFPSCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AHealthPickup::AHealthPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Root Collision detects the player.
	RootCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RootCollision"));

	//** Set the collision box as the root component. 
	SetRootComponent(RootCollision);

	//**Set the box size and detect pawn overlaps without affecting navigation. 
	RootCollision->InitBoxExtent(FVector(50.0f));
	RootCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RootCollision->SetCollisionObjectType(ECC_WorldDynamic);
	RootCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootCollision->SetGenerateOverlapEvents(true);
	RootCollision->SetCanEverAffectNavigation(false);

	//**Call onpickupoverlap when something overlpas the box. 
	RootCollision->OnComponentBeginOverlap.AddDynamic(this, &AHealthPickup::OnPickupOverlap);

	//Both Meshes attach to the root collision
	BoxMeshOne = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMeshOne"));

	//**Attach the first mesh and disable its collision, overlaps, and navigation effects. 
	BoxMeshOne->SetupAttachment(RootCollision);
	BoxMeshOne->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxMeshOne->SetGenerateOverlapEvents(false);
	BoxMeshOne->SetCanEverAffectNavigation(false);

	//**Create the second mesh component
	BoxMeshTwo = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMeshTwo"));

	//**Attach the second mesh and disable its collision, overlaps, and navigation effects. 
	BoxMeshTwo->SetupAttachment(RootCollision);
	BoxMeshTwo->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxMeshTwo->SetGenerateOverlapEvents(false);
	BoxMeshTwo->SetCanEverAffectNavigation(false);

}

void AHealthPickup::OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//**Stop if the pickup has already been used. 
	if (bConsumed)
	{
		return;
	}

	AANye_HorrorFPSCharacter* Player = Cast<AANye_HorrorFPSCharacter>(OtherActor);

	//Stop if this overlap wasn't caused by the player. 
	if (!IsValid(Player))
	{
		return;
	}

	//**Try to restore 15 health and store the amount restored. 
	const float RestoredHealth = Player->Heal(15.0f);

	//** Mark the pickup as used and remove it if health was restored. 
	if (RestoredHealth > 0.0f)
	{
		bConsumed = true;

		RootCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		Destroy();
	}
}



