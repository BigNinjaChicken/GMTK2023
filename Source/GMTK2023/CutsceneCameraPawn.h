// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CutsceneCameraPawn.generated.h"

UCLASS()
class GMTK2023_API ACutsceneCameraPawn : public APawn
{
 GENERATED_BODY()

public:
    ACutsceneCameraPawn();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(EditAnywhere, Category = "Cutscene Camera")
    TArray<AActor*> CameraLocations;

    UPROPERTY(EditAnywhere, Category = "Cutscene Camera")
    float TransitionTime;

    UFUNCTION()
    void MoveCameraToNextLocation();

    void StartCutscene();

    void MoveCameraToLocation(FVector TargetLocation);

    void TransitionCompleted();

    void NotifyOtherPlayers();
};
