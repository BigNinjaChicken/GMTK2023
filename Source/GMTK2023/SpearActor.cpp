// Fill out your copyright notice in the Description page of Project Settings.


#include "SpearActor.h"
#include <Components/CapsuleComponent.h>
#include <Components/StaticMeshComponent.h>


// Sets default values
ASpearActor::ASpearActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the hit box component and attach it to the root component
	Hitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Hitbox"));
	RootComponent = Hitbox;

	// Create the spear mesh component
	SpearMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpearMesh"));

	// Attach SpearMesh to the Hitbox component
	SpearMesh->SetupAttachment(Hitbox);

}

// Called when the game starts or when spawned
void ASpearActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpearActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

