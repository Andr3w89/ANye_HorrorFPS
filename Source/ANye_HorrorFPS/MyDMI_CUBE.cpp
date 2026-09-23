// Fill out your copyright notice in the Description page of Project Settings.


#include "MyDMI_CUBE.h"
#include "ANye_HorrorFPSCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


// Sets default values
AMyDMI_CUBE::AMyDMI_CUBE()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>("Box Component");
	cubeMesh = CreateDefaultSubobject<UStaticMeshComponent>("Cube Mesh");

	RootComponent = boxComp;
	cubeMesh->SetupAttachment(boxComp);

}

// Called when the game starts or when spawned
void AMyDMI_CUBE::BeginPlay()
{
	Super::BeginPlay();

	boxComp->OnComponentBeginOverlap.AddDynamic(this, &AMyDMI_CUBE::OnOverlapBegin);

	if (baseMat)
	{
		dmiMat = UMaterialInstanceDynamic::Create(baseMat, this);
	}

	if (cubeMesh)
	{
		cubeMesh->SetMaterial(0, dmiMat);
	}
	
}

// Called every frame
void AMyDMI_CUBE::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyDMI_CUBE::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AANye_HorrorFPSCharacter* overlappedActor = Cast<AANye_HorrorFPSCharacter>(OtherActor);

	if (overlappedActor)
	{
		float ranNumX = UKismetMathLibrary::RandomFloatInRange(0.0f, 1.0f);
		float ranNumY = UKismetMathLibrary::RandomFloatInRange(0.0f, 1.0f);
		float ranNumZ = UKismetMathLibrary::RandomFloatInRange(0.0f, 1.0f);

		FLinearColor randColor = FLinearColor(ranNumX, ranNumY, ranNumZ, 1.0f);
		if (dmiMat)
		{
			dmiMat->SetVectorParameterValue("Color", randColor);
			dmiMat->SetScalarParameterValue("Darkness", ranNumX);

			if (colorP)
			{
				UNiagaraComponent* particleComp = UNiagaraFunctionLibrary::SpawnSystemAttached(colorP, OtherComp, NAME_None, FVector(0.0f), FRotator(0.0f), EAttachLocation::KeepRelativeOffset, true);

				particleComp->SetNiagaraVariableLinearColor(FString("RandColor"), randColor);
			}
			
		}
	}
}



