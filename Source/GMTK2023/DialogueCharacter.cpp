// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueCharacter.h"
#include <UMG/Public/Components/WidgetComponent.h>
#include <Components/BoxComponent.h>
#include "SpearCharacter.h"
#include <Components/SphereComponent.h>

// Sets default values
ADialogueCharacter::ADialogueCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ButtonPromptWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ButtonPromptWidget"));
	ButtonPromptWidget->SetupAttachment(RootComponent);
	ButtonPromptWidget->SetVisibility(false);

	TriggerVolume = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(RootComponent);
	TriggerVolume->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void ADialogueCharacter::BeginPlay()
{
	Super::BeginPlay();

	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ADialogueCharacter::OnOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &ADialogueCharacter::OnOverlapEnd);
}

// Called every frame
void ADialogueCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADialogueCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ADialogueCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->IsA(ASpearCharacter::StaticClass()))
		return;

	ButtonPromptWidget->SetVisibility(true);

}

void ADialogueCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor->IsA(ASpearCharacter::StaticClass()))
		return;

	ButtonPromptWidget->SetVisibility(false);

}

