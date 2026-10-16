// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "ElectricBall.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Components/SceneComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//** Set up automatic AI Controll for the enemy
	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	//**Stop the controller from directly turning the enemy
	bUseControllerRotationYaw = false;

	//**Make the enemy face its movement direction and set its speed
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 200.0f;

	//**Creates the projectile spawn point
	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnPoint"));

	//**Attach the spawn point to the root component.
	ProjectileSpawnPoint->SetupAttachment(GetRootComponent());
	
	//Forward of the capsule, around upper body height
	ProjectileSpawnPoint->SetRelativeLocation(FVector(100.0f, 0.0f, 40.0f));

}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	//**Starts the enemy with full health
	CurrentHealth = MaxHealth;

	//**Saves the enemy's starting location
	HomeLocation = GetActorLocation();

	//Pick a New Destination every four seconds.
	GetWorldTimerManager().SetTimer(WanderTimer, this, &AEnemyCharacter::Wander, 4.0f, true, 0.2f);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemyCharacter::TryThrowBall, FMath::Max(AttackInterval, 0.1f), true);
	
}

//** Clear the timers when the enemy leaves the game.
void AEnemyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(WanderTimer);
	GetWorldTimerManager().ClearTimer(AttackTimer);

	Super::EndPlay(EndPlayReason);
}

//**Get the enemy's AI controller to handle wandering.
void AEnemyCharacter::Wander()
{
	AAIController* AIController = Cast<AAIController>(GetController());

	//**Get the navigaation system used to find reachable locations.
	UNavigationSystemV1* Navigation = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (!AIController || !Navigation)
	{
		return;
	}

	//**Find a random reachaable location near home and move there
	FNavLocation Destination;

	if (Navigation->GetRandomReachablePointInRadius(HomeLocation, WanderRadius, Destination))
	{
		AIController->MoveToLocation(Destination.Location, 100.0f);
	}
}

//**Try to throw enemy projectile at the player
void AEnemyCharacter::TryThrowBall()
{
	//**gets the player pawn
	APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);

	//**Gets the enemy's AI controller
	AAIController* AIController = Cast<AAIController>(GetController());

	if (!Player || !AIController || !ElectricBallClass)
	{
		return;
	}

	//**Stops the attack if player is out of range
	if (FVector::DistSquared(GetActorLocation(), Player->GetActorLocation()) > FMath::Square(AttackRange))
	{
		return;
	}

	//Dont fire at a player hidden behind a wall.
	if (!AIController->LineOfSightTo(Player))
	{
		return;
	}

	//**Get the projectile spawn point's world location
	const FVector SpawnLocation = ProjectileSpawnPoint->GetComponentLocation();

	//**Calculates the direction toward the player
	const FVector Direction = (Player->GetActorLocation() - SpawnLocation).GetSafeNormal();

	//** stop the attack if there is no valid direction
	if (Direction.IsNearlyZero())
	{
		return;
	}

	//**SEt the enemy as the projectiles owner and instigator
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = this;

	//**Adjust the spawn location if blocked or cancel if still blocked
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	//**Spawn the electric ball facing the plaayer
	GetWorld()->SpawnActor<AElectricBall>(ElectricBallClass, SpawnLocation, Direction.Rotation(), SpawnParameters);
}

//**Handles incoming damage and return the health lost
float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (DamageAmount <= 0.0f || CurrentHealth <= 0.0f)
	{
		return 0.0f;
	}

	//**Let the paarent class process the damage
	const float AcceptedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//**Save the health before taking damage
	const float PreviousHealth = CurrentHealth;

	//**Subtracts the damage and keeps health between zero and max
	CurrentHealth = FMath::Clamp(CurrentHealth - FMath::Max(AcceptedDamage, 0.0f), 0.0f, MaxHealth);

	//**Calculate how much health was lost. 
	const float ActualDamage = PreviousHealth - CurrentHealth;

	UE_LOG(LogTemp, Warning, TEXT("%s health: %.1f"), *GetName(), CurrentHealth);

	//**Remove the enemy when its health is depleted. 
	if (CurrentHealth <= 0.0f)
	{
		Destroy();
	}

	return ActualDamage;


}


