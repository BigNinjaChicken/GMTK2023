#include "BrittleObjectComponent.h"
#include "TimerManager.h"
#include "SpearCharacter.h"
#include "SpearActor.h"
#include "Engine/Engine.h" 

// Sets default values for this component's properties
UBrittleObjectComponent::UBrittleObjectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UBrittleObjectComponent::BeginPlay()
{
	Super::BeginPlay();

	// Bind overlap event to handle the spear touching the brittle object
	if (GetOwner())
	{
		GetOwner()->OnActorBeginOverlap.AddDynamic(this, &UBrittleObjectComponent::OnSpearOverlap);
	}
}

// Called every frame
void UBrittleObjectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TickAddBrittleTexture();
}

void UBrittleObjectComponent::OnSpearOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Stop from running twice
	if (bIsBrittle)
		return;

	ASpearActor* SpearActor = Cast<ASpearActor>(OtherActor);
	ASpearCharacter* SpearCharacter = Cast<ASpearCharacter>(OtherActor);

	if (SpearActor || SpearCharacter)
	{
		bIsBrittle = true;

		// Start the timer for invisibility
		GetWorld()->GetTimerManager().SetTimer(InvisibilityTimerHandle, this, &UBrittleObjectComponent::MakeInvisible, VisibleDuration, false);
	}
}

void UBrittleObjectComponent::MakeInvisible()
{
	// Make the actor invisible and untouchable
	// Example implementation:
	AActor* OwnerActor = GetOwner();
	if (OwnerActor)
	{
		OwnerActor->SetActorHiddenInGame(true);
		OwnerActor->SetActorEnableCollision(false);

		// Destroy all attached spear actors
		TArray<AActor*> AttachedActors;
		OwnerActor->GetAttachedActors(AttachedActors);
		for (AActor* AttachedActor : AttachedActors)
		{
			if (ASpearActor* SpearActor = Cast<ASpearActor>(AttachedActor))
			{
				SpearActor->Destroy();
			}
		}
	}

	bIsInvisible = true;

	// Start the timer to revert the visibility change
	GetWorld()->GetTimerManager().SetTimer(InvisibilityTimerHandle, this, &UBrittleObjectComponent::MakeVisible, InvisibleDuration, false);
}

void UBrittleObjectComponent::MakeVisible()
{
	// Make the actor visible and touchable again
	// Example implementation:
	AActor* OwnerActor = GetOwner();
	if (OwnerActor)
	{
		OwnerActor->SetActorHiddenInGame(false);
		OwnerActor->SetActorEnableCollision(true);
	}

	bIsInvisible = false;
	bIsBrittle = false;
}



void UBrittleObjectComponent::TickAddBrittleTexture()
{
	if (!bIsBrittle)
		return;

	// Perform additional actions when the actor is brittle but not invisible
	// Example: Apply a brittle texture
}