// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPortal.h"
#include "ANye_HorrorFPSCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyPortal::AMyPortal()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//** Creates the portal mesh, overlap box, capture camera and direction arrow.
	mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	boxComp = CreateDefaultSubobject<UBoxComponent>("Box Comp");
	sceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>("Capture");
	rootArrow = CreateDefaultSubobject<UArrowComponent>("Root Arrow");

	//**Makes the box the root and attaches the other components to build the portal. 
	RootComponent = boxComp;
	mesh->SetupAttachment(boxComp);
	sceneCapture->SetupAttachment(mesh);
	rootArrow->SetupAttachment(RootComponent);

	//**Ensures the mesh ignores collsions in order to not block the player. 
	mesh->SetCollisionResponseToAllChannels(ECR_Ignore);

}

// Called when the game starts or when spawned
void AMyPortal::BeginPlay()
{
	Super::BeginPlay();
	//** Connects box overlaps to OnOverlapBegin and hides the mesh from scene capture. 
	boxComp->OnComponentBeginOverlap.AddDynamic(this, &AMyPortal::OnOverlapBegin);
	mesh->SetHiddenInSceneCapture(true);

	//**Safety check to ensure the applied material is assigned. 
	if (mat)
	{
		mesh->SetMaterial(0, mat);
	}

}

// Called every frame
void AMyPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//**Updates the portals capture camera every frame. 
	UpdateMyPortals();

}

void AMyPortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AANye_HorrorFPSCharacter* playerChar = Cast<AANye_HorrorFPSCharacter>(OtherActor);

	//**Checks if the overlapping actor is the expected player characater. 
	if (playerChar)
	{
		//**Checks if a destination portal has been assigned. 
		if (OtherPortal)
		{
			//**Checks to ensure cool down has been done before allowing another teleport. 
			if (!playerChar->isTeleporting)
			{
				//**Starts the cooldown and gets the destination arrow's position and rotation. 
				playerChar->isTeleporting = true;
				FVector loc = OtherPortal->rootArrow->GetComponentLocation();
				FRotator rotation = OtherPortal->rootArrow->GetComponentRotation();
				//**This was used early on before I adjusted the code. Can still be used to change the players direction upon teleport. 
				rotation.Yaw += 0.0f;
				rotation.Pitch = 0.0f;
				rotation.Roll = 0.0f;

				//**Moves and turns the player to match the destination position and rotation. 
				playerChar->SetActorLocationAndRotation(loc, rotation);

				//**Turns the player's view to match the exit direction and checks if controller exists. 
				if (AController* controller = playerChar->GetController())
				{
					controller->SetControlRotation(rotation);
				}

				//**Schedules SetBool to run once after one second so the player can teleport again. 
				FTimerHandle TimerHandle;
				FTimerDelegate TimerDelegate;
				TimerDelegate.BindUFunction(this, "SetBool", playerChar);
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 1, false);
			}
		}
	}

}

//**Checks if player exists, and ends cool down allowing player to telleport again. 
void AMyPortal::SetBool(AANye_HorrorFPSCharacter* playerChar)
{
	if (playerChar)
	{
		playerChar->isTeleporting = false;
	}
}

//** updates this portal's capture camera, stopping if camera or arrow is missing. 
void AMyPortal::UpdateMyPortals()
{
	if (!sceneCapture || !rootArrow)
	{
		return;
	}

	//** Reads the portal's arrow's current position and rotation. 
	FVector captureLocation = rootArrow->GetComponentLocation();
	FRotator captureRotation = rootArrow->GetComponentRotation();

	//** Places and turns the capture camera to match the arrow. 
	sceneCapture->SetWorldLocationAndRotation(captureLocation, captureRotation);
}

