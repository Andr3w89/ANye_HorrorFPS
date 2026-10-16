// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

//**
class AElectricBall;
class USceneComponent;

UCLASS()
class ANYE_HORRORFPS_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

	//**Declare the function that handles incoming damage. 
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//**Declare the function that clears timers when the enemy leaves the game
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//**Store the component where projectiles spawns. 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USceneComponent> ProjectileSpawnPoint;

	//**Set the electric ball class used for attacks. 
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AElectricBall> ElectricBallClass;

	//**Sets the time in seconds between attack attempts. 
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (ClampMin = "0.1"))
	float AttackInterval = 2.0f;

	//**Set the max distance for attack
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (ClampMin = "0.0"))
	float AttackRange = 2000.0f;

	//**Set how faar the enemy can wander from home
	UPROPERTY(EditAnywhere, Category = "Movement", meta = (ClampMin = "0.0"))
	float WanderRadius = 1000.0f;


	//**sets Enemys max health
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", meta = (ClampMin = "1.0"))
	float MaxHealth = 100.0f;

	//**Stores the enemies current health. 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 100.0f;


private:
	//**Store the enemys starting location
	FVector HomeLocation;

	//**Store handles for the wandering and attack timers. 
	FTimerHandle WanderTimer;
	FTimerHandle AttackTimer;

	//**Declar the functions for wandering and attack. 
	void Wander();
	void TryThrowBall();

};
