// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"

#include "SpearCharacter.generated.h"

UCLASS()
class GMTK2023_API ASpearCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpearSpringArm;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ThrowAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RecallAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SwapAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* TalkAction;

	UPROPERTY(EditDefaultsOnly, Category = "G_Spear")
	TSubclassOf<class ASpearActor> SpearActorBlueprint;

	UPROPERTY(EditAnywhere, Category = "G_UI")
	TSubclassOf<class UUserWidget> CrosshairWidgetBlueprint;

	UPROPERTY(EditAnywhere, Category = "G_UI")
	TSubclassOf<class UUserWidget> DialogueWidgetBlueprint;

	UPROPERTY(EditAnywhere, Category = "G_Spear")
	TSubclassOf<class UCameraShakeBase> SpearHoldingCameraShakeBlueprint;

	UPROPERTY(EditAnywhere, Category = "G_Spear")
	TArray<TSubclassOf<class ASpearActor>> SpearActorBlueprints;

	UPROPERTY(EditAnywhere, Category = "G_Effects")
	class UNiagaraSystem* RecallPoofParticle;

	UPROPERTY(EditAnywhere, Category = "G_Effects")
	class UNiagaraComponent* PickUpCollectableParticle;

public:
	ASpearCharacter();

	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	virtual void PossessedBy(AController* NewController) override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	// Throw
	void Throw(const FInputActionValue& Value);
	void ResetThrowCooldown();
	void ThrowOngoing(const FInputActionValue& Value);
	FVector GetSpearTargetLocation();
	void ThrowComplete(const FInputActionValue& Value);
	void MoveCameraBoomBack();

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "G_SpearThrow")
	ASpearActor* NewSpear;

	UPROPERTY(EditAnywhere, Category = "G_SpearThrow")
	FVector SpearSpawnOffset = FVector(0.0f, 0.0f, 150.0f);

	UPROPERTY(EditAnywhere, Category = "G_SpearThrow")
	float RaycastMaxDistance = 10000.0f;

	UPROPERTY(EditAnywhere, Category = "G_SpearThrow")
	float CameraMoveBackSpeed = 500.0f;

	UPROPERTY(EditAnywhere, Category = "G_SpearThrow")
	float CameraNudgeMaxDistance = 75.0f;

	UPROPERTY(EditAnywhere, Category = "G_SpearThrow")
	float CameraNudgeSpeed = 1.0f;

	// Recall
	void Recall(const FInputActionValue& Value);
	void RecallTick(float DeltaTime);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "G_SpearRecall")
	bool bRecalling = false;
	ASpearActor* HitSpear;
	double SpearStartingDistance;

	UPROPERTY(EditAnywhere, Category = "G_SpearRecall")
    float MoveSpeed = 400.0f;

	UPROPERTY(EditAnywhere, Category = "G_SpearRecall")
    float ArcHeight = 100.f;

	UPROPERTY(EditAnywhere, Category = "G_SpearRecall")
    float DeleteDelay = 0.2f;

	UPROPERTY(EditAnywhere, Category = "G_SpearRecall", Meta = (ToolTip = "The desired maximum scaling factor when the spear reaches the player. By decreasing the MaxScaleFactor, the spear will scale down more as it gets closer to the player."))
	float MaxScaleFactor = 0.1f; // Set this to the desired maximum scaling factor when the spear reaches the player

	UPROPERTY(EditAnywhere, Category = "G_SpearRecall")
	double SpearDeleteDistance = 150.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "G_SpearRecall")
	bool bIsThrowOnCooldown;
	FTimerHandle ThrowCooldownTimerHandle;

	UPROPERTY(EditAnywhere, Category = "G_SpearRecall")
	float CooldownDuration = 0.5f;

	// Swap
	void Swap(const FInputActionValue& Value);

	// Talk
	void Talk(const FInputActionValue& Value);

	UUserWidget* DialogueUserWidget;

	UFUNCTION(BlueprintImplementableEvent, Category = "G_Talk")
	void UpdateDialogText(const FText& InputText, class UUserWidget* InDialogueUserWidget);

	bool bHasEnteredDialogue = false;
	int32 CurrentDialogueIndex = 0;

	void DisplayDialogueText(class ADialogueCharacter* CurrentDialogueCharacter);

	void TickDialogueCamera(float DeltaTime);

	FTransform TargetCameraTransform;

	UPROPERTY(EditAnywhere, Category = "G_Talk")
    float CameraLerpSpeed = 5.0f;

    UPROPERTY(EditAnywhere, Category = "G_Talk")
    float CameraLerpThreshold = 0.05f;

	FTransform StaringCameraTransform;
public:
	void PickedUpCollectible();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "G_Collectible")
	int32 CollectiblesPickedUp = 0;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
