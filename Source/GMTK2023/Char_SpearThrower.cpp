// Fill out your copyright notice in the Description page of Project Settings.


#include "Char_SpearThrower.h"

// Sets default values
AChar_SpearThrower::AChar_SpearThrower()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AChar_SpearThrower::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChar_SpearThrower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AChar_SpearThrower::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

