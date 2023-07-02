// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EOS_GameMode.h"
#include "EOS_GM_Level1.generated.h"

/**
 * 
 */
UCLASS()
class GMTK2023_API AEOS_GM_Level1 : public AEOS_GameMode
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, Category = "G_LevelSequence")
    class ULevelSequence* LevelSequence;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "G_LevelSequence")
    class ULevelSequencePlayer* SequencePlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "G_LevelSequence")
    class ALevelSequenceActor* SequenceActor;
};
