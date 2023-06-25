#include "BreakableActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SpearActor.h"

// Sets default values
ABreakableActor::ABreakableActor()
{
	// Set this actor to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	// Create the mesh component and set it as the root component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	// Enable overlap events for the mesh component
	MeshComponent->SetGenerateOverlapEvents(true);
	MeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	// Set default values for particle effect and sound
	ParticleEffect = nullptr;
	BreakSound = nullptr;
}

// Called when the game starts or when spawned
void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();

	// Bind the OnOverlapBegin function to the overlap event of the mesh component
	MeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ABreakableActor::OnOverlapBegin);
}

// Called every frame
void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABreakableActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the overlapping actor is of type ASpearActor
	if (OtherActor && OtherActor->IsA(ASpearActor::StaticClass()))
	{
		// Spawn the particle effect at the actor's location
		if (ParticleEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleEffect, GetActorTransform().GetLocation());
		}

		// Play the sound
		if (BreakSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), BreakSound, GetActorLocation());
		}

		// Destroy the actor
		Destroy();
	}
}
