// SequencePawn.cpp

#include "SequencePawn.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"

ASequencePawn::ASequencePawn()
{
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	RootComponent = RootSceneComponent;

	BookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BookMesh"));
	BookMesh->SetupAttachment(RootComponent);

	PageMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PageMesh"));
	PageMesh->SetupAttachment(BookMesh);

	FlipPageAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("FlipPageAudio"));
	FlipPageAudio->SetupAttachment(BookMesh);
}

void ASequencePawn::BeginPlay()
{
	Super::BeginPlay();

	// Add Input Mapping Context
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}

		PlayerController->SetShowMouseCursor(true);
	}

	ShowNextPage();
}

void ASequencePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASequencePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(SkipSceneAction, ETriggerEvent::Triggered, this, &ASequencePawn::SkipScene);
	}
}

void ASequencePawn::SkipScene(const FInputActionValue& Value)
{
	ShowNextPage();
}

void ASequencePawn::ShowNextPage()
{
	if (PageIndex >= Pages.Num()) {
		UGameplayStatics::OpenLevel(GetWorld(), LevelName);
		return;
	}

	FlipPageAudio->Play();
	PageMesh->SetMaterial(0, Pages[PageIndex]);

	PageIndex++;
}
