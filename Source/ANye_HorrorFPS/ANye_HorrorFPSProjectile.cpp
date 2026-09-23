
// Copyright Epic Games, Inc. All Rights Reserved.

#include "ANye_HorrorFPSProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AANye_HorrorFPSProjectile::AANye_HorrorFPSProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AANye_HorrorFPSProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	ballMesh = CreateDefaultSubobject<UStaticMeshComponent>("Ball Mesh");

	// Set as root component
	RootComponent = CollisionComp;

	ballMesh->SetupAttachment(CollisionComp);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 5000.f;
	ProjectileMovement->MaxSpeed = 5000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}
//Initialize the projectile's random color and dynamic material at play begins.
void AANye_HorrorFPSProjectile::BeginPlay()
{
	Super::BeginPlay();
	//Generate random red, green, and blue values with an alpha value
	randColor = FLinearColor(UKismetMathLibrary::RandomFloatInRange(0.0f, 1.0f), UKismetMathLibrary::RandomFloatInRange(0.0f, 1.0f), UKismetMathLibrary::RandomFloatInRange(0.0f, 1.0f), 1.0f);

	//Create a dynamic material instance so this projectile can change its material params.
	dmiMat = UMaterialInstanceDynamic::Create(projMat, this);
	//Apply the dynmaic material to the projectiles mesh. 
	ballMesh->SetMaterial(0, dmiMat);
	//Pass the random color to the material's projColor vector param.
	dmiMat->SetVectorParameterValue("projColor", randColor);
}

void AANye_HorrorFPSProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}

	if (OtherActor != nullptr)
	{
		//** Check that a Niagara system asset is assigned before creating the particle effect. 
		if (colorP)
		{
			//**Spawn the Niagara effect attached to HitComp
			UNiagaraComponent* particleComp = UNiagaraFunctionLibrary::SpawnSystemAttached(colorP, HitComp, NAME_None, FVector(-20.0f, 0.0f, 0.0f), FRotator(0.0f), EAttachLocation::KeepRelativeOffset, true);
			//**Pass the projectiles random color to the Niagara effect's RandColor param.
			particleComp->SetNiagaraVariableLinearColor(FString("RandColor"), randColor);
			//Remove the projectiles mesh component after spawning impact effect.
			ballMesh->DestroyComponent();
			//**Switch the collision sphere to no collision to prevent more interactions.
			CollisionComp->BodyInstance.SetCollisionProfileName("NoCollision");
		}

		//Generate random floating point value for the decal material's frame param. 
		float frameNum = UKismetMathLibrary::RandomFloatInRange(0.0f, 3.0f);
		
		//Spaw a decal with a uniform random size. 
		auto Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), baseMat, FVector(UKismetMathLibrary::RandomFloatInRange(20.0f, 40.0f)), Hit.Location, Hit.Normal.Rotation(), 0.0f);
		//Create a dynamic material instance
		auto MatInstance = Decal->CreateDynamicMaterialInstance();
		//Give the decal the same random color as the projectile.
		MatInstance->SetVectorParameterValue("Color", randColor);
		//Pass the random frame value to the decal to control the frame number.
		MatInstance->SetScalarParameterValue("Frame", frameNum);
	}
}
