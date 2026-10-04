// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portal_Class.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class USceneCaptureComponent2D;

UCLASS()
class ANYE_HORRORFPS_API APortal_Class : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal_Class();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Capture")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Capture")
	UStaticMeshComponent* DisplayMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Capture")
	USceneCaptureComponent2D* SceneCapture;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
