// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthPickup.generated.h"

//**
class UBoxComponent;
class UStaticMeshComponent;
class UPrimitiveCommponent;

UCLASS()
class ANYE_HORRORFPS_API AHealthPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHealthPickup();

protected:

	//**Store the collision box that detects the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> RootCollision;

	//**Store the pickup's first mesh component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BoxMeshOne;

	//**Store the pickups second mesh component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent>BoxMeshTwo;

private:
	//**Track whether the pickup has already been used. 
	bool bConsumed = false;

	//**Declare the function called when something overlpas the pickup. 
	UFUNCTION()
	void OnPickupOverlap(UPrimitiveComponent* OverLappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
