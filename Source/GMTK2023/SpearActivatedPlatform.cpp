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
}

// Called every frame
void ASpearActivatedPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsTouchingSpearActor())
	{
		StopMovement();
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
		StopMovement();
		return;
	}

	ParticleSystemComponent->Activate();

	FVector CurrentLocation = GetActorLocation();
	FVector TargetLocation = GetActorLocation() + PlatformLocations[TargetLocationIndex];

	// Calculate the direction and distance to move
	FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
	float Distance = FVector::Distance(CurrentLocation, TargetLocation);

	// Move the platform towards the target location
	SetActorLocation(CurrentLocation + Direction * MovementSpeed * DeltaTime);

	// Check if the platform has reached the target location
	if (Distance <= MovementSpeed * DeltaTime)
	{
		SetActorLocation(TargetLocation);
		StopMovement();
	}
}

void ASpearActivatedPlatform::StopMovement()
{
	ParticleSystemComponent->Deactivate();

	// Slowly halt the platform instead of stopping instantly
	TargetLocationIndex = (TargetLocationIndex + 1) % PlatformLocations.Num();
}