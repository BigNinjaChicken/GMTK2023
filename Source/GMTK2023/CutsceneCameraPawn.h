// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CutsceneCameraPawn.generated.h"

UCLASS()
class GMTK2023_API ACutsceneCameraPawn : public APawn
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
    ACutsceneCameraPawn();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void RunCutscene(float DeltaTime);

    void CompleteCutscene();

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "G_Cutscene")
    TArray<class ACutsceneLocation*> CutsceneLocations;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "G_Cutscene")
    int32 CurrentIndex = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "G_Cutscene")
    float ElapsedTime = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "G_Cutscene")
    bool bCutsceneComplete = false;

    UPROPERTY(EditDefaultsOnly, Category = "G_Cutscene")
	TSubclassOf<class ASpearCharacter> SpearCharacterBlueprint;
};
