// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "PerlinRockDeform.generated.h"

class UStaticMesh;

//**Editable geometry for one material section
struct FRockSectionData
{
	int32 SectionIndex = 0;

	TArray<FVector> Vertices;
	TArray<FVector> OriginalVertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;
};

UCLASS()
class ANYE_HORRORFPS_API APerlinRockDeform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APerlinRockDeform();

	//**
	virtual void OnConstruction(const FTransform& Transform) override;

	//**Pass the world-space impact position and outwaard surface normal.
	UFUNCTION(BlueprintCallable, Category = "Rock")
	void HitRock(FVector ImpactPoint, FVector ImpactNormal, float DamageAmount);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rock")
	UStaticMesh* RockMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rock|Damage", meta = (ClampMin = "1.0"))
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Rock|Damage")
	float CurrentHealth = 100.0f;

	//These Distances are in world units.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rock|Deformation", meta = (ClampMin = "0.1"))
	float DentRadius = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rock|Deformation", meta = (ClampMin = "0.0"))
	float DentDepth = 3.0f;

	//Limits total Vertex Movement to reduce extreme distortion.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rock|Deformation", meta = (ClampMin = "0.0"))
	float MaxDentDistance = 12.0f;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	UPROPERTY(VisibleAnywhere, Category = "Rock")
	UProceduralMeshComponent* ProcMesh = nullptr;

	TArray<FRockSectionData> RockSections;

	bool BuildRockMesh();

	void ApplyDent(const FVector& ImpactPoint, const FVector& ImpactNormal);

};
