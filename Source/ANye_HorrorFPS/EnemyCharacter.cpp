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

	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 200.0f;

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnPoint"));

	ProjectileSpawnPoint->SetupAttachment(GetRootComponent());
	
	//Forward of the capsule, around upper body height
	ProjectileSpawnPoint->SetRelativeLocation(FVector(100.0f, 0.0f, 40.0f));

}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	HomeLocation = GetActorLocation();

	//Pick a New Destination every four seconds.
	GetWorldTimerManager().SetTimer(WanderTimer, this, &AEnemyCharacter::Wander, 4.0f, true, 0.2f);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemyCharacter::TryThrowBall, FMath::Max(AttackInterval, 0.1f), true);
	
}

void AEnemyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(WanderTimer);
	GetWorldTimerManager().ClearTimer(AttackTimer);

	Super::EndPlay(EndPlayReason);
}

void AEnemyCharacter::Wander()
{
	AAIController* AIController = Cast<AAIController>(GetController());

	UNavigationSystemV1* Navigation = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (!AIController || !Navigation)
	{
		return;
	}

	FNavLocation Destination;

	if (Navigation->GetRandomReachablePointInRadius(HomeLocation, WanderRadius, Destination))
	{
		AIController->MoveToLocation(Destination.Location, 50.0f);
	}
}

void AEnemyCharacter::TryThrowBall()
{
	APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);

	AAIController* AIController = Cast<AAIController>(GetController());

	if (!Player || !AIController || !ElectricBallClass)
	{
		return;
	}

	if (FVector::DistSquared(GetActorLocation(), Player->GetActorLocation()) > FMath::Square(AttackRange))
	{
		return;
	}

	//Dont fire at a player hidden behind a wall.
	if (!AIController->LineOfSightTo(Player))
	{
		return;
	}

	const FVector SpawnLocation = ProjectileSpawnPoint->GetComponentLocation();

	const FVector Direction = (Player->GetActorLocation() - SpawnLocation).GetSafeNormal();

	if (Direction.IsNearlyZero())
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = this;

	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	GetWorld()->SpawnActor<AElectricBall>(ElectricBallClass, SpawnLocation, Direction.Rotation(), SpawnParameters);
}

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (DamageAmount <= 0.0f || CurrentHealth <= 0.0f)
	{
		return 0.0f;
	}

	const float AcceptedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	const float PreviousHealth = CurrentHealth;

	CurrentHealth = FMath::Clamp(CurrentHealth - FMath::Max(AcceptedDamage, 0.0f), 0.0f, MaxHealth);

	const float ActualDamage = PreviousHealth - CurrentHealth;

	UE_LOG(LogTemp, Warning, TEXT("%s health: %.1f"), *GetName(), CurrentHealth);

	if (CurrentHealth <= 0.0f)
	{
		Destroy();
	}

	return ActualDamage;


}


