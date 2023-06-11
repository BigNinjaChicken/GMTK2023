#include "LoginPawn.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/PlayerController.h"

// Sets default values
ALoginPawn::ALoginPawn()
{
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALoginPawn::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocallyControlled())
    {
        // Create widget
        UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), MainMenu);
        if (Widget)
        {
            Widget->AddToViewport();

            // Get player controller
            APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
            if (PlayerController)
            {
                // Show cursor
                PlayerController->bShowMouseCursor = true;

                // Set input mode UI only
				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(Widget->TakeWidget());
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				PlayerController->SetInputMode(InputMode);
            }
        }
    }
}

// Called every frame
void ALoginPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
