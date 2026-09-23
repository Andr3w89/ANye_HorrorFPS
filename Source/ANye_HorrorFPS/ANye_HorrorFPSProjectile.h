// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ANye_HorrorFPSProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;

UCLASS(config = Game)
class AANye_HorrorFPSProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	//Stores the mesh component that displays the projectile
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ballMesh;

	//Stores the splatter decal in the editor
	UPROPERTY(EditAnywhere, Category = "Effects")
	UMaterial* baseMat;

	//Stores the random color for the projectile and impact effects. 
	UPROPERTY()
	FLinearColor randColor;

	//Stores the projectiles material used for the color of the projectile.
	UPROPERTY(EditAnywhere)
	UMaterialInterface* projMat;

	//Stores the projectile material instance
	UPROPERTY()
	UMaterialInstanceDynamic* dmiMat;

	//**Stores the Niagara system asset for the projectiles impact effect.
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* colorP;

//Allows other code to use the constructor
public:
	AANye_HorrorFPSProjectile();

//keeps this startup function available to this class and subclass
protected:
	virtual void BeginPlay();

//Allows other code to use the hit handler and component getters.
public:

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};
