// Fill out your copyright notice in the Description page of Project Settings.


#include "PerlinProcTerrain.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"

// Sets default values
APerlinProcTerrain::APerlinProcTerrain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//**Creates the procedural mesh component and makes it the actors root.
	//**Enable Collsion and block visibility traces. 
	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>("Procedural Mesh");
	SetRootComponent(ProcMesh);

	ProcMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	ProcMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

// Called when the game starts or when spawned
void APerlinProcTerrain::BeginPlay()
{
	Super::BeginPlay();

	//**Generates the grid's vertices and traingle connections and uses arrays to create a mesh section.
	//**The true enables collision for the section. 
	CreateVertices();
	CreateTriangles();
	ProcMesh->CreateMeshSection(sectionID, Vertices, Triangles, Normals, UV0, UpVertexColors, TArray<FProcMeshTangent>(), true);
	ProcMesh->SetMaterial(0, Mat);
	
}

// Called every frame
void APerlinProcTerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APerlinProcTerrain::AlterMesh(FVector impactPoint)
{
	//**Checks the vertex and subtracts the actor's position from the hit position.
	for (int i = 0; i < Vertices.Num(); i++)
	{
		FVector tempVector = impactPoint - this->GetActorLocation();

		//**Measures the 3D distance from the vertex to the hit anhd subtracts the depth.
		if (FVector(Vertices[i] - tempVector).Size() < radius)
		{
			Vertices[i] = Vertices[i] - Depth;
			ProcMesh->UpdateMeshSection(sectionID, Vertices, Normals, UV0, UpVertexColors, TArray<FProcMeshTangent>());
		}
	}
}

void APerlinProcTerrain::CreateVertices()
{
	//**The two for loops create the (XSize + 1) and (YSize + 1) vertices. 
	for (int X = 0; X <= XSize; X++)
	{
		
		for (int Y = 0; Y <= YSize; Y++)
		{
			//**Grabs the vertex height, NoiseScale controls sampling spacing and offsets the sample.
			float Z = FMath::PerlinNoise2D(FVector2D(X * NoiseScale + 0.1, Y * NoiseScale + 0.1)) * ZMultiplier;
			GEngine->AddOnScreenDebugMessage(-1, 999.0f, FColor::Yellow, FString::Printf(TEXT("Z %f"), Z));
			Vertices.Add(FVector(X * Scale, Y * Scale, Z));
			UV0.Add(FVector2D(X * UVScale, Y * UVScale));
		}
	}
}

void APerlinProcTerrain::CreateTriangles()
{
	//**Tracks the vertex index at the starting corner of the current cell.
	int Vertex = 0;

	//**Visits each X cell but excludes the final cell. 
	for (int X = 0; X < XSize; X++)
	{
		//**Visits each Y cell within the current X strip.
		for (int Y = 0; Y < YSize; Y++)
		{
			//**Connects the cells four courners using two triangles, adding 1 moves to the next Y vertex,
			//**And adding (+ 1) moves to the next X Strip. 
			Triangles.Add(Vertex);
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + YSize + 1);
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + YSize + 2);
			Triangles.Add(Vertex + YSize + 1);

			//**Advcances to the next cell vertex along Y
			Vertex++;
		}
		//**Skip final boundary vertex to start next interation of the X strip.
		Vertex++;
	}

}

