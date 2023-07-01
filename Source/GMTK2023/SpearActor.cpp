// Fill out your copyright notice in the Description page of Project Settings.


#include "SpearActor.h"
#include <Components/CapsuleComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/TimelineComponent.h>
#include "Engine/World.h"
#include <Components/SceneComponent.h>
#include <Engine/HitResult.h>
#include "Components/BoxComponent.h"
#include <Particles/ParticleSystemComponent.h>
#include <Components/AudioComponent.h>
#include "HardObject.h"
#include "NiagaraComponent.h"

// Sets default values
ASpearActor::ASpearActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	SpearCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Hitbox"));
	SpearCollision->SetupAttachment(RootSceneComponent);

	SpearMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpearMesh"));
	SpearMesh->SetupAttachment(SpearCollision);

	SpearTipHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("SpearTipHitbox"));
	SpearTipHitbox->SetupAttachment(SpearCollision);

	// Create Particle components
	SpearEmittionParticle = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SpearEmittionParticle"));
	SpearEmittionParticle->SetupAttachment(SpearMesh);
	SpearEmittionParticle->SetRelativeLocation(FVector::ZeroVector);
	SpearEmittionParticle->bAutoActivate = true;

	SpearTrailParticle = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SpearTrailParticle"));
	SpearTrailParticle->SetupAttachment(SpearMesh);
	SpearTrailParticle->SetRelativeLocation(FVector::ZeroVector);
	SpearTrailParticle->bAutoActivate = false;

	// Audio Components
	MetalBounceAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("MetalBounceAudio"));
	MetalBounceAudio->SetupAttachment(SpearMesh);
	MetalBounceAudio->SetAutoActivate(false);

	ThrownAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("ThrownAudio"));
	ThrownAudio->SetupAttachment(SpearMesh);
	ThrownAudio->SetAutoActivate(false);

	HoldingAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("HoldingAudio"));
	HoldingAudio->SetupAttachment(SpearMesh);
	HoldingAudio->SetAutoActivate(true);

	ImpactAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("ImpactAudio"));
	ImpactAudio->SetupAttachment(SpearMesh);
	ImpactAudio->SetAutoActivate(false);
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

	if (!bIsThrown)
	{
		// Deactivate SpearTrailParticle when spear is not thrown
		if (SpearTrailParticle->IsActive())
		{
			SpearTrailParticle->Deactivate();
		}

		// Do not continue is spear is not thrown
		return;
	}

	if (!bHasThrownEffectsPlayed)
	{
		PlaySpearThrowEffects();
		bHasThrownEffectsPlayed = true;
	}

	FVector CurrentLocation = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();

	// Move the spear in the direction it is facing at a constant speed
	CurrentLocation += ForwardVector * ThrowStrength * DeltaTime;

	// Apply gravity effect by reducing the Z coordinate of the spear's location
	CurrentLocation.Z -= FallSpeed * DeltaTime;

	// Update the spear's location
	SetActorLocation(CurrentLocation);

	// Perform raycast to check if the SpearTipHitbox touches something
	FHitResult HitResult;
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + ForwardVector * ThrowStrength * DeltaTime;

	// Ignore collisions with the owner of the spear
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams))
	{
		AActor* HitActor = HitResult.GetActor();
		if (!HitActor)
			return;

		UHardObject* HardObject = HitActor->GetComponentByClass<UHardObject>();
		if (HardObject)
		{
			// The SpearTipHitbox touched a "Hard" tagged object, simulate bounce

			// Calculate the reflection direction
			FVector IncomingDirection = ForwardVector;
			FVector Normal = HitResult.Normal;
			FVector ReflectedDirection = IncomingDirection - 2 * FVector::DotProduct(IncomingDirection, Normal) * Normal;

			// Rotate the spear to face the reflected direction
			FRotator NewRotation = ReflectedDirection.Rotation();
			SetActorRotation(NewRotation);

			// Sound
			MetalBounceAudio->Play();

			return;
		}

		// The SpearTipHitbox touched something, stop moving and get stuck in the object it hit
		bIsThrown = false;
		SetActorLocation(HitResult.Location);

		FVector WallNormal = HitResult.Normal;

		// Rotate Actor to look into the hit location
		FRotator NewRotation = WallNormal.Rotation();
		NewRotation.Pitch = -NewRotation.Pitch;
		NewRotation.Yaw += 180.0f;
		SetActorRotation(NewRotation);

		// Calculate the new location to adjust the spear into the wall
		FVector WallOffset = WallNormal * OffsetFromWall;
		FVector NewLocation = HitResult.Location + WallOffset;
		SetActorLocation(NewLocation);

		// Attach the spear to the actor it hit
		AttachToActor(HitActor, FAttachmentTransformRules::KeepWorldTransform);

		// Impact Camera Shake
		FVector ShakeLocation = GetActorLocation();

		if (ImpactAudio) {
			ImpactAudio->Play();
		}

		for (FConstPlayerControllerIterator PlayerControllerIt = GetWorld()->GetPlayerControllerIterator(); PlayerControllerIt; ++PlayerControllerIt)
		{
			APlayerController* PlayerController = PlayerControllerIt->Get();
			APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager;
			PlayerCameraManager->StartCameraShake(SpearImpactCameraShakeBlueprint);
		}

		// Print the collided actor's name to the screen
		// FString CollidedActorName = HitResult.GetActor() != nullptr ? HitResult.GetActor()->GetName() : TEXT("Unknown Actor");
		// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, FString::Printf(TEXT("Collided with: %s"), *CollidedActorName));
	}
}

void ASpearActor::PlaySpearThrowEffects()
{
	if (SpearTrailParticle) {
		SpearTrailParticle->Activate();
	}

	if (ThrownAudio) {
		ThrownAudio->Play();
	}

	if (HoldingAudio) {
		HoldingAudio->Stop();
	}
}

