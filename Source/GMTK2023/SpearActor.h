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
    // The static mesh component for the spear
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spear")
    class UStaticMeshComponent* SpearMesh;

    // The hitbox component for the spear
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spear")
    class UCapsuleComponent* SpearCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spear")
    class UCapsuleComponent* SpearTipHitbox;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Throw")
    bool bIsThrown = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Throw")
    float ThrowStrength = 2000.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Throw")
	float FallSpeed = 100.0f;
};
