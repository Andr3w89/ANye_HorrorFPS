// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProcMesh_Static.generated.h"

UCLASS()
class ANYE_HORRORFPS_API AProcMesh_Static : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProcMesh_Static();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//
	virtual void PostActorCreated() override;
	virtual void PostLoad() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	//
	UPROPERTY()
	TArray<FVector> Vertices;

	//
	UPROPERTY()
	TArray<int> Triangles;
	
	//
	UPROPERTY()
	TArray<FVector> Normals;

	TArray<FVector2D> UV0;

	//
	UPROPERTY()
	TArray<FLinearColor> VertexColors;

	TArray<FColor> UpVertexColors;

	TArray<FProcMeshTangent> Tangents;

	//
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* baseMesh;

private:
	//
	UProceduralMeshComponent* procMesh;
	void GetMeshData();
	void CreateMesh();
};
