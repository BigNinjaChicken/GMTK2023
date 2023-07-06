#include "CutsceneCameraPawn.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"
#include "GameFramework/PlayerStart.h"
#include "CutsceneLocation.h"
#include <Components/SplineComponent.h>
#include <Components/ArrowComponent.h>
#include <Camera/CameraComponent.h>
#include "SpearCharacter.h"
#include "GameFramework/PlayerController.h" 


ACutsceneCameraPawn::ACutsceneCameraPawn()
{
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(RootComponent);
}

void ACutsceneCameraPawn::BeginPlay()
{
	Super::BeginPlay();

	// Get all CutsceneLocation actors
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACutsceneLocation::StaticClass(), OutActors);

	// Cast them and add them to an array
	for (AActor* Actor : OutActors)
	{
		ACutsceneLocation* CutsceneLocation = Cast<ACutsceneLocation>(Actor);
		if (CutsceneLocation)
		{
			CutsceneLocations.Add(CutsceneLocation);
		}
	}

	// Sort CutsceneLocations array by OrderIndex
	CutsceneLocations.Sort([](const ACutsceneLocation& A, const ACutsceneLocation& B) {
		return A.OrderIndex < B.OrderIndex;
		});
}

void ACutsceneCameraPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

	RunCutscene(DeltaTime);
}

void ACutsceneCameraPawn::RunCutscene(float DeltaTime)
{
	// Check if there are any CutsceneLocations available
	if (CutsceneLocations.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("CutsceneLocations Is Empty"));
		return;
	}

	if (CurrentIndex >= CutsceneLocations.Num())
	{
		if (!bCutsceneComplete)
		{
			// The cutscene is complete, perform any necessary actions
			CompleteCutscene();
			bCutsceneComplete = true;
		}
		return;
	}

	ACutsceneLocation* CutsceneLocation = CutsceneLocations[CurrentIndex];
	int32 OrderIndex = CutsceneLocation->OrderIndex;
	USplineComponent* SplineComponent = CutsceneLocation->CameraSplineTrack;
	float TotalTime = CutsceneLocation->TotalTime;

	// Check if the spline component is valid
	if (!SplineComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("SplineComponent is null"));
		return;
	}

	// Calculate the target distance along the spline based on OrderIndex
	float TargetDistance = SplineComponent->GetSplineLength() * (static_cast<float>(OrderIndex) / CutsceneLocations.Num());

	// Calculate the current distance along the spline based on the elapsed time
	float CurrentDistance = TargetDistance * (ElapsedTime / TotalTime);

	// Calculate the target location along the spline
	FVector TargetLocation = SplineComponent->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);

	// Set the camera's location to the target location
	SetActorLocation(TargetLocation);

	// Set the camera's rotation based on the Arrow rotation in the CutsceneLocation
	FRotator TargetRotation = CutsceneLocation->ArrowComponent->GetComponentRotation();
	FollowCamera->SetWorldRotation(TargetRotation);

	// Update the elapsed time
	ElapsedTime += DeltaTime;

	// Check if the total time has been reached
	if (ElapsedTime >= TotalTime)
	{
		CurrentIndex++;
		ElapsedTime = 0.0f;
	}
}

void ACutsceneCameraPawn::CompleteCutscene()
{

	// Log the completion of the cutscene
	UE_LOG(LogTemp, Warning, TEXT("Cutscene complete. SpearCharacter spawned and possessed."));

	// Spawn the ASpearCharacter
	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = FRotator::ZeroRotator; // Reset rotation to zero
	FActorSpawnParameters SpawnParams;
	ASpearCharacter* SpearCharacter = GetWorld()->SpawnActor<ASpearCharacter>(SpearCharacterBlueprint, SpawnLocation, SpawnRotation, SpawnParams);

	if (SpearCharacter)
	{
		// Possess the SpearCharacter with the player controller
		APlayerController* PlayerController = Cast<APlayerController>(Controller);
		PlayerController->Possess(SpearCharacter);

	}
}