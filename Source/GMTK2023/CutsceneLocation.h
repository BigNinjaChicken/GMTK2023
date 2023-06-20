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

	UPROPERTY(EditAnywhere, Category = "G_Cutscene")
	class USplineComponent* CameraSplineTrack;

	UPROPERTY(EditAnywhere, Category = "G_Cutscene")
    class UArrowComponent* ArrowComponent;

	UPROPERTY(EditAnywhere, Category = "G_Cutscene", meta = (ClampMin = "1", ClampMax = "10", UIMin = "1", UIMax = "10", SliderExponent = "1.0"))
	int32 OrderIndex = 1;

	UPROPERTY(EditAnywhere, Category = "G_Cutscene", meta = (ClampMin = "1", ClampMax = "10", UIMin = "1", UIMax = "10", SliderExponent = "1.0"))
	float TotalTime = 4.0f;
};
