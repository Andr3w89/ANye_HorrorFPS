// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal_Class.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"

// Sets default values
APortal_Class::APortal_Class()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	DisplayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayMesh"));
	DisplayMesh->SetupAttachment(SceneRoot);

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->SetupAttachment(SceneRoot);

	SceneCapture->bCaptureEveryFrame = true;
	SceneCapture->bCaptureOnMovement = true;

	SceneCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;

	//Prevents camera from capturing its own display.
	SceneCapture->HiddenComponents.Add(DisplayMesh);

}

// Called when the game starts or when spawned
void APortal_Class::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APortal_Class::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

