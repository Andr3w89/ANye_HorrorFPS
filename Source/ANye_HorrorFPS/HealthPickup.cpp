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

	SetRootComponent(RootCollision);

	RootCollision->InitBoxExtent(FVector(50.0f));
	RootCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RootCollision->SetCollisionObjectType(ECC_WorldDynamic);
	RootCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootCollision->SetGenerateOverlapEvents(true);
	RootCollision->SetCanEverAffectNavigation(false);

	RootCollision->OnComponentBeginOverlap.AddDynamic(this, &AHealthPickup::OnPickupOverlap);

	//Both Meshes attach to the root collision
	BoxMeshOne = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMeshOne"));

	BoxMeshOne->SetupAttachment(RootCollision);
	BoxMeshOne->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxMeshOne->SetGenerateOverlapEvents(false);
	BoxMeshOne->SetCanEverAffectNavigation(false);

	BoxMeshTwo = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMeshTwo"));

	BoxMeshTwo->SetupAttachment(RootCollision);
	BoxMeshTwo->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxMeshTwo->SetGenerateOverlapEvents(false);
	BoxMeshTwo->SetCanEverAffectNavigation(false);

}

void AHealthPickup::OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
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

	const float RestoredHealth = Player->Heal(15.0f);

	if (RestoredHealth > 0.0f)
	{
		bConsumed = true;

		RootCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		Destroy();
	}
}



