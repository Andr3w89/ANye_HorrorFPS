// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VertKeyDoor.generated.h"

//** Declare the component classes used by the door
class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class ANYE_HORRORFPS_API AVertKeyDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVertKeyDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//** Store the door's root component.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	TObjectPtr<USceneComponent> SceneRoot;

	//** store the door's mesh component.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	//** Set the key ID needed to unlock the door.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	FName RequiredKeyID = TEXT("Door1");

	//** Set how hight the door moves when opened.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door", meta = (ClampMin = "0.0"))
	float OpenHeight = 300.0f;

	//**Sets how fast the door opens.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door", meta = (ClampMin = "1.0"))
	float OpenSpeed = 150.0f;

	//** Sets how close the player must be to open the door. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door", meta = (ClampMin = "0.0"))
	float InteractionDistance = 300.0f;

private:
	//**Stores the door's closed and open positions.
	FVector ClosedRelativeLocation = FVector::ZeroVector;
	FVector OpenRelativeLocation = FVector::ZeroVector;
	FVector ClosedWorldLocation = FVector::ZeroVector;

	//**Track whether the door is opening or already opened. 
	bool bOpening = false;
	bool bOpened = false;

	//**Declares the function that checks if the door can open. 
	void TryOpen();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
