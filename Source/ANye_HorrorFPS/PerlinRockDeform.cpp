// Fill out your copyright notice in the Description page of Project Settings.


#include "PerlinRockDeform.h"
#include "Engine/StaticMesh.h"
#include "KismetProceduralMeshLibrary.h"

// Sets default values
APerlinRockDeform::APerlinRockDeform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural Mesh"));

	SetRootComponent(ProcMesh);
	
	ProcMesh->bUseComplexAsSimpleCollision = true;

	ProcMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	ProcMesh->SetCollisionObjectType(ECC_WorldStatic);
	ProcMesh->SetCollisionResponseToAllChannels(ECR_Block);

}

void APerlinRockDeform::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	BuildRockMesh();
}

// Called when the game starts or when spawned
void APerlinRockDeform::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = FMath::Max(1.0f, MaxHealth);

	BuildRockMesh();
	
}

bool APerlinRockDeform::BuildRockMesh()
{
	ProcMesh->ClearAllMeshSections();
	RockSections.Reset();

	if (!RockMesh)
	{
		return false;
	}

	if (!RockMesh->bAllowCPUAccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Enable Allow CPU Access on %s."), *GetName(), *RockMesh->GetName());

		return false;
	}

	const int32 LODIndex = 0;
	const int32 SectionCount = RockMesh->GetNumSections(LODIndex);

	for (int32 SectionIndex = 0; SectionIndex < SectionCount; ++SectionIndex)
	{
		FRockSectionData Section;
		Section.SectionIndex = SectionIndex;

		UKismetProceduralMeshLibrary::GetSectionFromStaticMesh(RockMesh, LODIndex, SectionIndex, Section.Vertices, Section.Triangles, Section.Normals, Section.UVs, Section.Tangents);

		if (Section.Vertices.IsEmpty() || Section.Triangles.IsEmpty())
		{
			continue;
		}

		Section.OriginalVertices = Section.Vertices;

		ProcMesh->CreateMeshSection(SectionIndex, Section.Vertices, Section.Triangles, Section.Normals, Section.UVs, TArray<FColor>(), Section.Tangents, true);

		//Sections can reference different material slots.
		const int32 MaterialIndex = RockMesh->GetSectionInfoMap().Get(LODIndex, SectionIndex).MaterialIndex;

		ProcMesh->SetMaterial(SectionIndex, RockMesh->GetMaterial(MaterialIndex));

		RockSections.Add(MoveTemp(Section));

	}

	return !RockSections.IsEmpty();
}

void APerlinRockDeform::HitRock(FVector ImpactPoint, FVector ImpactNormal, float DamageAmount)
{
	if (CurrentHealth <= 0.0f || DamageAmount <= 0.0f || RockSections.IsEmpty())
	{
		return;
	}

	ApplyDent(ImpactPoint, ImpactNormal);

	CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);

	if (CurrentHealth <= 0.0f)
	{
		Destroy();
	}
}

void APerlinRockDeform::ApplyDent(const FVector& ImpactPoint, const FVector& ImpactNormal)
{
	if (DentRadius <= 0.0f || DentDepth <= 0.0f || MaxDentDistance <= 0.0f)
	{
		return;
	}

	//Trace impact normals point outward.
	const FVector InwardDirection = -ImpactNormal.GetSafeNormal();

	if (InwardDirection.IsNearlyZero())
	{
		return;
	}

	const FTransform MeshTransform = ProcMesh->GetComponentTransform();

	for (FRockSectionData& Section : RockSections)
	{
		bool bChanged = false;

		for (int32 Index = 0; Index < Section.Vertices.Num(); ++Index)
		{
			const FVector WorldPosition = MeshTransform.TransformPosition(Section.Vertices[Index]);

			const float Distance = FVector::Distance(WorldPosition, ImpactPoint);

			if (Distance >= DentRadius)
			{
				continue;
			}

			//Smooth falloff: Strongest at center.
			const float T = 1.0f - Distance / DentRadius;

			const float Falloff = T * T * (3.0f - 2.0f * T);

			const FVector ProposedWorldPosition = WorldPosition + InwardDirection * DentDepth * Falloff;

			const FVector OriginalWorldPosition = MeshTransform.TransformPosition(Section.OriginalVertices[Index]);

			//Limit cumulative deformation in world units.
			const FVector Offset = (ProposedWorldPosition - OriginalWorldPosition).GetClampedToMaxSize(MaxDentDistance);

			Section.Vertices[Index] = MeshTransform.InverseTransformPosition(OriginalWorldPosition + Offset);

			bChanged = true;
		}

		if (!bChanged)
		{
			continue;
		}

		UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Section.Vertices, Section.Triangles, Section.UVs, Section.Normals, Section.Tangents);

		//Update once per affected section, after moving its vertices.
		ProcMesh->UpdateMeshSection(Section.SectionIndex, Section.Vertices, Section.Normals, Section.UVs, TArray<FColor>(), Section.Tangents);
	}
}


