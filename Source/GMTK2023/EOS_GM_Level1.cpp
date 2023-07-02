// Fill out your copyright notice in the Description page of Project Settings.

#include "EOS_GM_Level1.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "MovieSceneSequencePlaybackSettings.h"

void AEOS_GM_Level1::BeginPlay()
{
    Super::BeginPlay();

    if (LevelSequence)
    {
        // Create a new level sequence player instance
        SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), LevelSequence, FMovieSceneSequencePlaybackSettings(), SequenceActor);

        if (SequencePlayer)
        {
            // Set the sequence player's owner and play the sequence
            SequencePlayer->Play();
        }
    }
}
