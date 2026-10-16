
// Copyright Epic Games, Inc. All Rights Reserved.

#include "ANye_HorrorFPSProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "PerlinRockDeform.h"
#include "PerlinProcTerrain.h"
#include "EnemyCharacter.h"
#include "GameFramework/DamageType.h"

AANye_HorrorFPSProjectile::AANye_HorrorFPSProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AANye_HorrorFPSProjectile::OnHit);		

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

//** Handle projectile hits, enemy damage, and impact effects. 
void AANye_HorrorFPSProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	if (!OtherActor || OtherActor == this || OtherActor == GetOwner() || OtherActor == GetInstigator())
	{
		return;
	}

	//** Start with the enemy hit flag set to false. 
	bool bHitEnemy = false;

	UE_LOG(LogTemp, Warning, TEXT("Player projectile hit: %s"), *GetNameSafe(OtherActor));

	//**If an enemy was hit, mark the hit, disable collision, and deal 20 damage. 
	if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(OtherActor))
	{

		bHitEnemy = true;
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		UGameplayStatics::ApplyDamage(Enemy, 20.0f, GetInstigatorController(), this, UDamageType::StaticClass());

	}

	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}

	//**Create impact effects and remove the projectile if the hit actor exists. 
	if (OtherActor != nullptr)
	{
		//**Disable further projectile collisions. 
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//**Spawn the impact particles and apply the projectiles random color. 
		if (colorP)
		{
			UNiagaraComponent* ParticleComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), colorP, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());

			if (ParticleComp)
			{
				ParticleComp->SetVariableLinearColor(FName(TEXT("RandColor")), randColor);
			}
		}

		//** Spawn an impact decal with a random size and frame value. 
		if (baseMat)
		{
			const float FrameNum = UKismetMathLibrary::RandomFloatInRange(0.0f, 3.0f);

			const float DecalSize = UKismetMathLibrary::RandomFloatInRange(20.0f, 40.0f);

			UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), baseMat, FVector(DecalSize), Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), 0.0f);

			//** Create a dynamic material for the decal if it spawned. 
			if (Decal)
			{
				UMaterialInstanceDynamic* MatInstance = Decal->CreateDynamicMaterialInstance();

				//**Apply the random color and frame value to the decal material. 
				if (MatInstance)
				{
					MatInstance->SetVectorParameterValue(TEXT("Color"), randColor);

					MatInstance->SetScalarParameterValue(TEXT("Frame"), FrameNum);
				}
			}

		}

		Destroy();
	}
}
