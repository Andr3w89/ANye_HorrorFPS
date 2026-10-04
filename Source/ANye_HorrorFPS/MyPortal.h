// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/BoxComponent.h"
#include "ANye_HorrorFPSCharacter.h"
#include "Components/ArrowComponent.h"
#include "MyPortal.generated.h"

//** Declares character class before the portal class.
class ANye_HorrorFPSCharacter;

UCLASS()
class ANYE_HORRORFPS_API AMyPortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyPortal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//** Holds the portal's mesh. 
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* mesh;

	//**Holds the camera component that captures the view for the portal. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneCaptureComponent2D* sceneCapture;

	//**Holds an arrow thata marks the exit position and direction which guides the camera. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* rootArrow;

	//**Holds a texture that can receive the captured view. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextureRenderTarget2D* renderTarget;

	//**Holds the box that uses overlap to detect actors entering. 
	UPROPERTY(EditAnywhere)
	UBoxComponent* boxComp;

	//**Holds the destination portal that the actor will be transported to. 
	UPROPERTY(EditAnywhere)
	AMyPortal* OtherPortal;

	//**Holds the material that is applied to the portal mesh. 
	UPROPERTY(EditAnywhere)
	UMaterialInterface* mat;

	//**Declares the function called when an actor is overlapping the portal box. 
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//**Declares the function that allows the player to teleport again after a set cooldown. 
	UFUNCTION()
	void SetBool(AANye_HorrorFPSCharacter* playerChar);

	//**Declares the function that moves the captures camer to match the portals arrow. 
	UFUNCTION()
	void UpdateMyPortals();


};
