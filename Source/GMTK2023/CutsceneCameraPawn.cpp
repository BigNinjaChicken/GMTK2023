#include "CutsceneCameraPawn.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"


ACutsceneCameraPawn::ACutsceneCameraPawn()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ACutsceneCameraPawn::BeginPlay()
{
    Super::BeginPlay();

    // Wait for other players to load into the level
    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, this, &ACutsceneCameraPawn::StartCutscene, 1.0f, false);
}

void ACutsceneCameraPawn::MoveCameraToNextLocation()
{
    if (CameraLocations.IsValidIndex(0))
    {
        FVector TargetLocation = CameraLocations[0]->GetActorLocation();
        CameraLocations.RemoveAt(0);
        MoveCameraToLocation(TargetLocation);
    }
    else
    {
        // All camera locations have been visited, cutscene is complete
        TransitionCompleted();
    }
}

void ACutsceneCameraPawn::StartCutscene()
{
    // Notify other players that the cutscene is starting
    NotifyOtherPlayers();

    // Start moving the camera to the next location
    MoveCameraToNextLocation();
}

void ACutsceneCameraPawn::MoveCameraToLocation(FVector TargetLocation)
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        PlayerController->SetViewTargetWithBlend(this, TransitionTime);
        SetActorLocation(TargetLocation);
        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(TimerHandle, this, &ACutsceneCameraPawn::MoveCameraToNextLocation, TransitionTime, false);
    }
}

void ACutsceneCameraPawn::TransitionCompleted()
{
    // Cutscene is complete, return control to the player
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        PlayerController->Possess(PlayerController->GetPawn());
        Destroy();
    }
}

void ACutsceneCameraPawn::NotifyOtherPlayers()
{
    TArray<AActor*> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

    for (AActor* PlayerStart : PlayerStarts)
    {
        if (PlayerStart != nullptr && PlayerStart != this)
        {
            APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), PlayerStarts.Find(PlayerStart));
            if (PlayerController)
            {
                // Notify other players to pause their gameplay
                PlayerController->SetPause(true);
            }
        }
    }
}