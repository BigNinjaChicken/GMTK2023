// Fill out your copyright notice in the Description page of Project Settings.


#include "CutsceneLocation.h"
#include <Components/SplineComponent.h>
#include "Components/ArrowComponent.h"

// Sets default values
ACutsceneLocation::ACutsceneLocation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CameraSplineTrack = CreateDefaultSubobject<USplineComponent>(TEXT("MySpline"));
	SetRootComponent(CameraSplineTrack);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->AttachToComponent(CameraSplineTrack, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void ACutsceneLocation::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACutsceneLocation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

