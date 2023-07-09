#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DialogueCharacter.generated.h"

USTRUCT(BlueprintType)
struct FSpeechText
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speech Text", meta = (MakeEditWidget = true))
	FTransform CameraTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speech Text")
	FText Dialogue;
};

UCLASS()
class GMTK2023_API ADialogueCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ADialogueCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class UWidgetComponent* ButtonPromptWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class USphereComponent* TriggerVolume;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "G_SpeechText")
	TArray<FSpeechText> SpeechTextArray;
};
