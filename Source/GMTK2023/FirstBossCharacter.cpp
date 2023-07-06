// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstBossCharacter.h"
#include <Components/StaticMeshComponent.h>
#include "SpearActor.h"
#include "Components/SkeletalMeshComponent.h"
#include <../Plugins/FX/Niagara/Source/Niagara/Classes/NiagaraSystem.h>
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>


// Sets default values
AFirstBossCharacter::AFirstBossCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Damage Crystal Mesh
	CrystalMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrystalMeshComponent"));
	CrystalMeshComponent->SetupAttachment(GetMesh());
	CrystalMeshComponent->SetGenerateOverlapEvents(true);
	CrystalMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AFirstBossCharacter::OnCrystalMeshBeginOverlap);

	CrystalEffectNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("CrystalEffectNiagaraComponent"));
	CrystalEffectNiagaraComponent->SetupAttachment(CrystalMeshComponent);
}

// OnBeginOverlap event handler for CrystalMeshComponent
void AFirstBossCharacter::OnCrystalMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor && !OtherActor->IsA(ASpearActor::StaticClass()))
	{
		return;
	}

	if (CurrentPhase == EBossFightPhase::NotStarted) {
		CurrentPhase = EBossFightPhase::Phase1;
		BeginPhase1();
	}
}

void AFirstBossCharacter::BeginPhase1()
{

}

void AFirstBossCharacter::BeginPhase2()
{

}

void AFirstBossCharacter::BeginPhase3()
{

}

// Called when the game starts or when spawned
void AFirstBossCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AFirstBossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

