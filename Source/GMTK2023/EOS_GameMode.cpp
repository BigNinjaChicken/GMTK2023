// Fill out your copyright notice in the Description page of Project Settings.

#include "EOS_GameMode.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"

void AEOS_GameMode::BeginPlay()
{
    Super::BeginPlay();
}

void AEOS_GameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    if (NewPlayer)
    {
        FUniqueNetIdRepl UniqueNetIdRepl;
        if (NewPlayer->IsLocalPlayerController()) // Host of server in GameMode
        {
            ULocalPlayer* LocalPlayerRef = NewPlayer->GetLocalPlayer();
            if (LocalPlayerRef)
            {
                UniqueNetIdRepl = LocalPlayerRef->GetPreferredUniqueNetId();
            }
            else
            {
                UNetConnection* RemoteNetConnectionRef = Cast<UNetConnection>(NewPlayer->Player);
                check(IsValid(RemoteNetConnectionRef));
                UniqueNetIdRepl = RemoteNetConnectionRef->PlayerId;
            }
        }
        else
        {
            UNetConnection* RemoteNetConnectionRef = Cast<UNetConnection>(NewPlayer->Player);
            check(IsValid(RemoteNetConnectionRef));
            UniqueNetIdRepl = RemoteNetConnectionRef->PlayerId;
        }

        TSharedPtr<const FUniqueNetId> UniqueNetId = UniqueNetIdRepl.GetUniqueNetId();
        if (UniqueNetId.IsValid())
        {
            IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(NewPlayer->GetWorld());
            IOnlineSessionPtr SessionRef = SubsystemRef->GetSessionInterface();
            bool bRegistrationSuccess = SessionRef->RegisterPlayer(FName("MainSession"), *UniqueNetId, false);
            if (bRegistrationSuccess)
            {
                UE_LOG(LogTemp, Warning, TEXT("Registration Success"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Invalid UniqueNetId"));
        }
    }
}
