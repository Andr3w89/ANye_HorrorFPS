// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VertKeyDoor.generated.h"

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	FName RequiredKeyID = TEXT("Door1");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door", meta = (ClampMin = "0.0"))
	float OpenHeight = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door", meta = (ClampMin = "1.0"))
	float OpenSpeed = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door", meta = (ClampMin = "0.0"))
	float InteractionDistance = 200.0f;

private:
	FVector ClosedRelativeLocation = FVector::ZeroVector;
	FVector OpenRelativeLocation = FVector::ZeroVector;
	FVector ClosedWorldLocation = FVector::ZeroVector;

	bool bOpening = false;
	bool bOpened = false;

	void TryOpen();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
