// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "PerlinProcTerrain.generated.h"

//**Forward declaration for ProceduralMeshComponent.
class UProceduralMeshComponent;
//**Forward declaration for UMaterialInterface.
class UMaterialInterface;

UCLASS()
class ANYE_HORRORFPS_API APerlinProcTerrain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APerlinProcTerrain();

	//**Number of grid cells along X.
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0))
	int XSize = 0;

	//**Number of grid cells along Y.
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0))
	int YSize = 0;

	//**Multiplies the noice value to control terrain height. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0))
	float ZMultiplier = 10.f;

	//**Controls the spacing between samples in the Perlin noise pattern.
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0))
	float NoiseScale = 1.0f;

	//**Distance between neighboring grid vertices along X and Y.
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.000001))
	float Scale = 0;

	//**Texture coordinate control spacing along the grid. 
	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.000001))
	float UVScale = 0;

	//**Distance around the raytrace that which vertices can be moved. 
	UPROPERTY(EditAnywhere)
	float radius;

	//**Displacement subtracked from each affected vertex. 
	UPROPERTY(EditAnywhere)
	FVector Depth;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//**Assigned material to the terrain slot. 
	UPROPERTY(EditAnywhere)
	UMaterialInterface* Mat;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	//**Moves vertices near the supplied world-space hit position.
	UFUNCTION()
	void AlterMesh(FVector impactPoint);

private:
	//**Displays the generated mesh and its collision aspects. 
	UProceduralMeshComponent* ProcMesh;
	//**Stores all grid vertices
	TArray<FVector> Vertices;
	//**Stores which three vertices connect to make each triangle.
	TArray<int> Triangles;
	//**Store the texture coords. that corespond to the vertices.
	TArray<FVector2D> UV0;
	//**Stores the surface direction for lighting. 
	TArray<FVector> Normals;
	//**Generates optional colors based on the vertices. 
	TArray<FColor> UpVertexColors;

	//**Identifies the mesh section that is created. 
	int sectionID = 0;

	//**Builds the vertex and texture components, and connects each grid cell corners into two triangles.  
	void CreateVertices();
	void CreateTriangles();

};
