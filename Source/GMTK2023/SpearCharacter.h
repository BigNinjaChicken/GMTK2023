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

	UPROPERTY(EditDefaultsOnly, Category = "G_Spear")
	TSubclassOf<class ASpearActor> SpearActorBlueprint;

	UPROPERTY(EditAnywhere, Category = "G_UI")
	TSubclassOf<class UUserWidget> CrosshairWidgetBlueprint;

public:
	ASpearCharacter();
	

protected:
	virtual void PossessedBy(AController* NewController) override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void Throw(const FInputActionValue& Value);
	void ThrowOngoing(const FInputActionValue& Value);
	void ThrowComplete(const FInputActionValue& Value);
	void MoveCameraBoomBack();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "G_Spear")
	ASpearActor* NewSpear;
	UPROPERTY(EditAnywhere, Category = "G_Spear")
	FVector SpearSpawnOffset = FVector(0.0f, 0.0f, 150.0f);
	UPROPERTY(EditAnywhere, Category = "G_Spear")
	float RaycastMaxDistance = 10000.0f;
	UPROPERTY(EditAnywhere, Category = "G_Spear")
	float CameraMoveBackSpeed = 500.0f;
	UPROPERTY(EditAnywhere, Category = "G_Spear")
	float CameraNudgeMaxDistance = 75.0f;
	UPROPERTY(EditAnywhere, Category = "G_Spear")
	float CameraNudgeSpeed = 1.0f;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
