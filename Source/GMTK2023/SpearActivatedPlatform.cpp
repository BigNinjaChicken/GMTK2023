#include "SpearActivatedPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include <Components/SceneComponent.h>
#include "SpearActor.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ASpearActivatedPlatform::ASpearActivatedPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create a static mesh component as the visual representation of the platform
	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	SetRootComponent(PlatformMesh);

	// Create a particle system component
	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	ParticleSystemComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ASpearActivatedPlatform::BeginPlay()
{
	Super::BeginPlay();

	FVector ActorLocation = GetActorLocation();
	for (FVector& Location : PlatformLocations)
	{
		Location += ActorLocation;
	}
}

// Called every frame
void ASpearActivatedPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsAlwaysMoving && !IsTouchingSpearActor())
	{
		return;
	}

	MoveToNextLocation(DeltaTime);
}


bool ASpearActivatedPlatform::IsTouchingSpearActor() const
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, ASpearActor::StaticClass());

	return OverlappingActors.Num() > 0;
}

void ASpearActivatedPlatform::MoveToNextLocation(float DeltaTime)
{
	if (PlatformLocations.Num() <= 0)
	{
		return;
	}

	ParticleSystemComponent->Activate();

	FVector CurrentLocation = GetActorLocation();
	FVector TargetLocation = PlatformLocations[TargetLocationIndex];

	FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
	float Distance = FVector::Distance(CurrentLocation, TargetLocation);

	SetActorLocation(CurrentLocation + Direction * MovementSpeed * DeltaTime);

	if (FVector::PointsAreNear(CurrentLocation, TargetLocation, 1.0f))
	{
		bCountingUp = (TargetLocationIndex >= PlatformLocations.Num() - 1) ? false : (TargetLocationIndex <= 0) ? true : bCountingUp;
		TargetLocationIndex = bCountingUp ? TargetLocationIndex + 1 : TargetLocationIndex - 1;
	}


}