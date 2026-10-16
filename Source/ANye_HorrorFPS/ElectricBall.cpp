// Fill out your copyright notice in the Description page of Project Settings.


#include "ElectricBall.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AElectricBall::AElectricBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//**Create the ball collsion sphere
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));

	//**Set the collision sphere as root component
	SetRootComponent(CollisionSphere);

	//**Sets collision size and blocks all channels except the camera. 
	CollisionSphere->InitSphereRadius(15.0f);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	//**Call onballhit when the collision sphere hits something
	CollisionSphere->OnComponentHit.AddDynamic(this, &AElectricBall::OnBallHit);

	//**Create the balls mesh component
	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));

	//**Attack and resize the mesh and disaable its collision
	BallMesh->SetupAttachment(CollisionSphere);
	BallMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BallMesh->SetRelativeScale3D(FVector(0.3f));

	//**Create the component thata moves the projectile
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	//**Set the ball's speed and rotation with no gravity or bounce. 
	ProjectileMovement->SetUpdatedComponent(CollisionSphere);
	ProjectileMovement->InitialSpeed = 1200.0f;
	ProjectileMovement->MaxSpeed = 1200.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->bShouldBounce = false;

	//**Destroys ball after five seconds. 
	InitialLifeSpan = 5.0f;

}

// Called when the game starts or when spawned
void AElectricBall::BeginPlay()
{
	Super::BeginPlay();

	//Lets the projectile leave the enemyu without hitting its shooter
	if (GetOwner())
	{
		CollisionSphere->IgnoreActorWhenMoving(GetOwner(), true);
	}

	//**Ignore the actor that fired the ball
	if (GetInstigator())
	{
		CollisionSphere->IgnoreActorWhenMoving(GetInstigator(), true);
	}
	
}

void AElectricBall::OnBallHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& hit)
{
	//**Skip repeaated hits and hits involving invalid actors.
	if (bHasHit || !OtherActor || OtherActor == this || OtherActor == GetOwner() || OtherActor == GetInstigator())
	{
		return;
	}

	//**Mark the ball as having hit something
	bHasHit = true;

	//Disable Collision first so this ball can only damage once.
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//**Deal 10 damage if the ball hits the player. 
	if (OtherActor == UGameplayStatics::GetPlayerPawn(this, 0))
	{
		UGameplayStatics::ApplyDamage(OtherActor, 10.0f, GetInstigatorController(), this, UDamageType::StaticClass());
	}

	Destroy();
}



