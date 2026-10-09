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

	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	SetRootComponent(PickupSphere);

	PickupSphere->InitSphereRadius(80.0f);
	PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	PickupSphere->SetGenerateOverlapEvents(true);

	KeyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KeyMesh"));
	KeyMesh->SetupAttachment(PickupSphere);
	KeyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void AKeyPickup::BeginPlay()
{
	Super::BeginPlay();
	
	PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &AKeyPickup::OnPickupOverlap);
	
}

void AKeyPickup::OnPickupOverlap(UPrimitiveComponent* OverlaappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	APawn* PlayerPawn = Cast<APawn>(OtherActor);

	if (bCollected || !PlayerPawn || !PlayerPawn->IsPlayerControlled() || KeyID.IsNone())
	{
		return;
	}

	//Prefix the ID to distinguish inventory keys from other actor taags.
	const FName InventoryTag(*FString::Printf(TEXT("Key.%s"), *KeyID.ToString()));

	PlayerPawn->Tags.AddUnique(InventoryTag);

	bCollected = true;
	Destroy();
}

// Called every frame
void AKeyPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

