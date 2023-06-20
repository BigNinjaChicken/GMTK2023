// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CutsceneLocation.generated.h"

UCLASS()
class GMTK2023_API ACutsceneLocation : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACutsceneLocation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Cutscene")
	class USplineComponent* CameraSplineTrack;

	UPROPERTY(EditAnywhere, Category = "Cutscene")
    class UArrowComponent* ArrowComponent;

	UPROPERTY(EditAnywhere, Category = "Cutscene")
	int OrderIndex = 0;
};
