#include "EOS_GM_Level1.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "LevelSequenceActor.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "GameFramework/PlayerController.h"
#include <Kismet/GameplayStatics.h>

void AEOS_GM_Level1::BeginPlay()
{
    Super::BeginPlay();

    // Disable player input for all PlayerControllers
    for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APlayerController* PlayerController = Cast<APlayerController>(*Iterator);
        if (PlayerController)
        {
            // Disable Input
            FInputModeUIOnly InputMode;
            PlayerController->SetInputMode(InputMode);
        }
    }

	if (bShouldPlayCutscene)
	{
        if (LevelSequence)
        {
            // Create a new level sequence player instance
            SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), LevelSequence, FMovieSceneSequencePlaybackSettings(), SequenceActor);

            if (SequencePlayer)
            {
                // Set the sequence player's owner and play the sequence
                SequencePlayer->OnFinished.AddDynamic(this, &AEOS_GM_Level1::OnSequenceFinished); // Bind the OnFinished event
                SequencePlayer->Play();
            }
        }
    }
    else 
    {
        OnSequenceFinished();
    }
}

void AEOS_GM_Level1::OnSequenceFinished()
{
    // Re-enable player input for all PlayerControllers
    for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APlayerController* PlayerController = Cast<APlayerController>(*Iterator);
        if (PlayerController)
        {
            // Switch input mode back to game input
            FInputModeGameOnly InputMode;
            PlayerController->SetInputMode(InputMode);
        }
    }   
}
