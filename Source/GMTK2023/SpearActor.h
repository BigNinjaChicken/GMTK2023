// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpearActor.generated.h"

UCLASS()
class GMTK2023_API ASpearActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpearActor();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class USceneComponent* RootSceneComponent;

    // The static mesh component for the spear
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
    class UStaticMeshComponent* SpearMesh;

    // The hitbox component for the spear
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
    class UBoxComponent* SpearCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
    class UCapsuleComponent* SpearTipHitbox;

	// Declare particle system components
	UPROPERTY(EditAnywhere, Category = "G_Effects")
	class UNiagaraComponent* SpearEmittionParticle;

	UPROPERTY(EditAnywhere, Category = "G_Effects")
	class UNiagaraComponent* SpearTrailParticle;

	UPROPERTY(EditAnywhere, Category = "G_Effects")
	TSubclassOf<class UCameraShakeBase> SpearImpactCameraShakeBlueprint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "G_Audio")
	UAudioComponent* MetalBounceAudio;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "G_Throw")
    bool bIsThrown = false;

	UPROPERTY(EditAnywhere, Category = "G_Throw")
    float ThrowStrength = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "G_Throw")
	float FallSpeed = 100.0f;

	UPROPERTY(EditAnywhere, Category = "G_Throw")
	float OffsetFromWall = 80.0f;
};
