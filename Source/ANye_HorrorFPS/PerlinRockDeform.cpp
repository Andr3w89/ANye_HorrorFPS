// Fill out your copyright notice in the Description page of Project Settings.


#include "PerlinRockDeform.h"
#include "Engine/StaticMesh.h"
#include "KismetProceduralMeshLibrary.h"

// Sets default values
APerlinRockDeform::APerlinRockDeform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//**Create the procedural mesh component. 
	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural Mesh"));

	//**Set the procedural mesh as the root component. 
	SetRootComponent(ProcMesh);
	
	//**Us the meshs triangles for collision. 
	ProcMesh->bUseComplexAsSimpleCollision = true;

	//**Enable collision queries and physics collision. 
	ProcMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	//**Treat the rock as a static object and block all collision channels. 
	ProcMesh->SetCollisionObjectType(ECC_WorldStatic);
	ProcMesh->SetCollisionResponseToAllChannels(ECR_Block);

}

//**Build the rock mesh when the actor is constructed. 
void APerlinRockDeform::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	BuildRockMesh();
}

// Called when the game starts or when spawned
void APerlinRockDeform::BeginPlay()
{
	Super::BeginPlay();

	//**Set the starting health to at least one. 
	CurrentHealth = FMath::Max(1.0f, MaxHealth);

	//**Build the rock mesh when the game starts. 
	BuildRockMesh();
	
}

//**Copy the static rock mesh into procedural mesh. 
bool APerlinRockDeform::BuildRockMesh()
{
	ProcMesh->ClearAllMeshSections();
	RockSections.Reset();

	//**Stop if no rock mesh is assigned. 
	if (!RockMesh)
	{
		return false;
	}

	//**Warn and stop if the mesh does not allow cpu access. 
	if (!RockMesh->bAllowCPUAccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Enable Allow CPU Access on %s."), *GetName(), *RockMesh->GetName());

		return false;
	}

	//**Get the number of sections in the mesh's first detail level. 
	const int32 LODIndex = 0;
	const int32 SectionCount = RockMesh->GetNumSections(LODIndex);

	//**Read the mesh data for eaach section. 
	for (int32 SectionIndex = 0; SectionIndex < SectionCount; ++SectionIndex)
	{
		FRockSectionData Section;
		Section.SectionIndex = SectionIndex;

		UKismetProceduralMeshLibrary::GetSectionFromStaticMesh(RockMesh, LODIndex, SectionIndex, Section.Vertices, Section.Triangles, Section.Normals, Section.UVs, Section.Tangents);

		//**Skip sections with no vertices or triangles. 
		if (Section.Vertices.IsEmpty() || Section.Triangles.IsEmpty())
		{
			continue;
		}

		//** Save the original vertex positions. 
		Section.OriginalVertices = Section.Vertices;

		//**Create the procedural mesh section with collision enabled. 
		ProcMesh->CreateMeshSection(SectionIndex, Section.Vertices, Section.Triangles, Section.Normals, Section.UVs, TArray<FColor>(), Section.Tangents, true);

		//Sections can reference different material slots.
		const int32 MaterialIndex = RockMesh->GetSectionInfoMap().Get(LODIndex, SectionIndex).MaterialIndex;

		//**Apply the original section's material. 
		ProcMesh->SetMaterial(SectionIndex, RockMesh->GetMaterial(MaterialIndex));

		//**Store the section data for later deformation
		RockSections.Add(MoveTemp(Section));

	}

	//** Return whether any mesh sections were created. 
	return !RockSections.IsEmpty();
}

//**Dent the rock, apply damage, aand destroy it when health runs out. 
void APerlinRockDeform::HitRock(FVector ImpactPoint, FVector ImpactNormal, float DamageAmount)
{
	//**Stop if the rock is destroyed damage is invalid or mesh data is missing. 
	if (CurrentHealth <= 0.0f || DamageAmount <= 0.0f || RockSections.IsEmpty())
	{
		return;
	}

	//**Create a dent at impact point. 
	ApplyDent(ImpactPoint, ImpactNormal);

	//**Subtract damage without letting health go below zero. 
	CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);

	//**Remove the rock when health reaches zero. 
	if (CurrentHealth <= 0.0f)
	{
		Destroy();
	}
}

//** Move nearby vertices inward to create a dent. 
void APerlinRockDeform::ApplyDent(const FVector& ImpactPoint, const FVector& ImpactNormal)
{
	//** Stop if any dent setting is zero or less. 
	if (DentRadius <= 0.0f || DentDepth <= 0.0f || MaxDentDistance <= 0.0f)
	{
		return;
	}

	//Trace impact normals point outward.
	const FVector InwardDirection = -ImpactNormal.GetSafeNormal();

	//**Stop if there is no valid inward direction. 
	if (InwardDirection.IsNearlyZero())
	{
		return;
	}

	//**Get the mesh transform to convert between locaal and world positions. 
	const FTransform MeshTransform = ProcMesh->GetComponentTransform();

	//** Check each rock section for vertices to dent. 
	for (FRockSectionData& Section : RockSections)
	{
		//** Track Whether any vertices in this section change. 
		bool bChanged = false;

		//** Find each vertex's world position and distance from the impact. 
		for (int32 Index = 0; Index < Section.Vertices.Num(); ++Index)
		{
			const FVector WorldPosition = MeshTransform.TransformPosition(Section.Vertices[Index]);

			const float Distance = FVector::Distance(WorldPosition, ImpactPoint);

			//** Skip vertices outside the dent radius. 
			if (Distance >= DentRadius)
			{
				continue;
			}

			//Smooth falloff: Strongest at center.
			const float T = 1.0f - Distance / DentRadius;

			//** Calculate a smooth dent strength based on distance. 
			const float Falloff = T * T * (3.0f - 2.0f * T);

			//** Calculate the vertex's proposed position after moving inward. 
			const FVector ProposedWorldPosition = WorldPosition + InwardDirection * DentDepth * Falloff;

			//** Get the vertex's original world position. 
			const FVector OriginalWorldPosition = MeshTransform.TransformPosition(Section.OriginalVertices[Index]);

			//Limit cumulative deformation in world units.
			const FVector Offset = (ProposedWorldPosition - OriginalWorldPosition).GetClampedToMaxSize(MaxDentDistance);

			//** Save the limited dent position in the mesh's local coordinates. 
			Section.Vertices[Index] = MeshTransform.InverseTransformPosition(OriginalWorldPosition + Offset);

			//** Mark this section as changed. 
			bChanged = true;
		}

		//**Skip updating sections thata did not change. 
		if (!bChanged)
		{
			continue;
		}

		//**Recalculate normals and tangents for the changed mesh. 
		UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Section.Vertices, Section.Triangles, Section.UVs, Section.Normals, Section.Tangents);

		//Update once per affected section, after moving its vertices.
		ProcMesh->UpdateMeshSection(Section.SectionIndex, Section.Vertices, Section.Normals, Section.UVs, TArray<FColor>(), Section.Tangents);
	}
}


