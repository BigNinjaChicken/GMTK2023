#include "CollectibleActor.h"
#include <Components/StaticMeshComponent.h>
#include <Components/SphereComponent.h>
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>
#include <Components/AudioComponent.h>
#include "Kismet/GameplayStatics.h"	
#include "SpearActor.h"
#include "SpearCharacter.h"
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h>
#include <Components/PrimitiveComponent.h>
#include <GameFramework/Actor.h>

ACollectibleActor::ACollectibleActor()
{
	PrimaryActorTick.bCanEverTick = true;

	CarrotMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarrotMesh"));
	SetRootComponent(CarrotMesh);
	CarrotMesh->OnComponentBeginOverlap.AddDynamic(this, &ACollectibleActor::OnBeginOverlapMesh);

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetupAttachment(CarrotMesh);
	CollisionSphere->InitSphereRadius(50.0f);
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ACollectibleActor::OnBeginOverlapSphere);
	CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &ACollectibleActor::OnEndOverlapSphere);

	CarrotCryParticle = CreateDefaultSubobject<UNiagaraComponent>(TEXT("CarrotCryParticle"));
	CarrotCryParticle->SetupAttachment(CarrotMesh);
	CarrotCryParticle->SetRelativeLocation(FVector::ZeroVector);
	CarrotCryParticle->bAutoActivate = false;

	ScaredAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("ScaredAudio"));
	ScaredAudio->SetupAttachment(CarrotMesh);
	ScaredAudio->SetAutoActivate(false);

	BraveAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("BraveAudio"));
	BraveAudio->SetupAttachment(CarrotMesh);
	BraveAudio->SetAutoActivate(false);
}

void ACollectibleActor::BeginPlay()
{
	Super::BeginPlay();

	bBraveLoopingSound = true;
	PlayBraveAudio();
}

void ACollectibleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerController* FirstPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);;
	if (FirstPlayerController)
	{
		FRotator TargetRotation = (FirstPlayerController->GetPawn()->GetActorLocation() - GetActorLocation()).Rotation();
		FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, RotateSpeed);
		SetActorRotation(NewRotation);
	}
}

void ACollectibleActor::OnBeginOverlapMesh(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("ACollectibleActor: Begin Overlap"));
	
	ASpearActor* OverlappingSpear = Cast<ASpearActor>(OtherActor);
	if (!OverlappingSpear) {
		return;
	}

	if (bScaredLoopingSound) {
		OverlappingSpear->DestroyWithEffects();

		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		ASpearCharacter* SpearCharacter = Cast<ASpearCharacter>(PlayerController->GetPawn());
		if (SpearCharacter)
		{
			SpearCharacter->PickedUpCollectible();
		}

		// Destroy actor last
		DestroyWithEffects();
	}
}

void ACollectibleActor::OnBeginOverlapSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->IsA(APlayerController::StaticClass()))
		return;

	bScaredLoopingSound = true;
	bBraveLoopingSound = false;
	PlayScaredAudio();
	BraveAudio->Stop();
}

void ACollectibleActor::OnEndOverlapSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor->IsA(APlayerController::StaticClass()))
		return;

	bScaredLoopingSound = false;
	bBraveLoopingSound = true;
	PlayBraveAudio();
	ScaredAudio->Stop();
}


void ACollectibleActor::PlayScaredAudio()
{
	if (!bScaredLoopingSound) {
		return;
	}

	if (ScaredAudio)
	{
		float LoopDuration = ScaredLoopEndTime - ScaredLoopStartTime;
		float SoundDuration = ScaredAudio->Sound->Duration;

		ScaredAudio->Play(ScaredLoopStartTime);

		float StopTime = SoundDuration - ScaredLoopEndTime;
		GetWorldTimerManager().SetTimer(TimerHandle_StopSound, this, &ACollectibleActor::PlayScaredAudio, LoopDuration, false);
	}
}

void ACollectibleActor::PlayBraveAudio()
{
	if (!bBraveLoopingSound) {
		return;
	}

	if (BraveAudio)
	{
		float LoopDuration = BraveLoopEndTime - BraveLoopStartTime;
		float SoundDuration = BraveAudio->Sound->Duration;

		BraveAudio->Play(BraveLoopStartTime);

		float StopTime = SoundDuration - BraveLoopEndTime;
		GetWorldTimerManager().SetTimer(TimerHandle_StopSound, this, &ACollectibleActor::PlayBraveAudio, LoopDuration, false);
	}
}

void ACollectibleActor::DestroyWithEffects()
{
	if (CarrotDeathParticle)
	{
		FVector ParticleSpawnLocation = GetActorLocation();
		FRotator ParticleSpawnRotation = FRotator::ZeroRotator;
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CarrotDeathParticle, ParticleSpawnLocation, ParticleSpawnRotation);
	}

	Destroy();
}


