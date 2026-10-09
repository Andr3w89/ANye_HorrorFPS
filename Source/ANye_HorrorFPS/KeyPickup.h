// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KeyPickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UPrimitiveComponent;

UCLASS()
class ANYE_HORRORFPS_API AKeyPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKeyPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Key")
	TObjectPtr<USphereComponent> PickupSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Key")
	TObjectPtr<UStaticMeshComponent> KeyMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key")
	FName KeyID = TEXT("Door1");


private: 
	bool bCollected = false;

	UFUNCTION()
	void OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
