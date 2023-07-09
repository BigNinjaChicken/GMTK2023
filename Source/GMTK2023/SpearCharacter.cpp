// Fill out your copyright notice in the Description page of Project Settings.


#include "SpearCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SpearActor.h"
#include <Engine/Engine.h>
#include <GameFramework/Actor.h>
#include "Math/UnrealMathUtility.h"
#include <UMG/Public/Blueprint/UserWidget.h>
#include <Camera/CameraShakeBase.h>
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraActor.h>
#include <UObject/UObjectGlobals.h>


ASpearCharacter::ASpearCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	SpearSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpearSpringArm"));
	SpearSpringArm->SetupAttachment(RootComponent);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	PickUpCollectableParticle = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PickUpCollectableParticle"));
	PickUpCollectableParticle->SetupAttachment(RootComponent);
	PickUpCollectableParticle->SetRelativeLocation(FVector::ZeroVector);
	PickUpCollectableParticle->bAutoActivate = false;
}

void ASpearCharacter::Tick(float DeltaTime)
{
	RecallTick(DeltaTime);
}

// BeginPlay Equivalent
void ASpearCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//Add Input Mapping Context
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (IsLocallyControlled())
	{
		// Create widget
		UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), CrosshairWidgetBlueprint);
		if (Widget)
		{
			Widget->AddToViewport();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASpearCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASpearCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASpearCharacter::Look);

		//Thowing Spear
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Started, this, &ASpearCharacter::Throw);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Ongoing, this, &ASpearCharacter::ThrowOngoing);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Completed, this, &ASpearCharacter::ThrowComplete);

		//Recalling Spear
		EnhancedInputComponent->BindAction(RecallAction, ETriggerEvent::Triggered, this, &ASpearCharacter::Recall);

		//Swapping Spear
		EnhancedInputComponent->BindAction(SwapAction, ETriggerEvent::Triggered, this, &ASpearCharacter::Swap);
	}
}

void ASpearCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ASpearCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ASpearCharacter::Throw(const FInputActionValue& Value)
{
	if (!SpearActorBlueprint)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpearActorBlueprint is null"));
		return;
	}

	// Check if the cooldown is still active
	if (bIsThrowOnCooldown)
	{
		UE_LOG(LogTemp, Warning, TEXT("Throw is on cooldown"));
		return;
	}

	FVector SpawnLocation = GetActorLocation() + SpearSpawnOffset; // Adjust the Z-axis offset as needed
	FVector SpearTargetLocation = GetSpearTargetLocation();

	FRotator SpawnRotation = (SpearTargetLocation - SpawnLocation).Rotation();
	NewSpear = nullptr;
	NewSpear = GetWorld()->SpawnActor<ASpearActor>(SpearActorBlueprint, SpawnLocation, SpawnRotation);

	if (!NewSpear)
	{
		UE_LOG(LogTemp, Warning, TEXT("NewSpear is null"));
		return;
	}

	// Attach the spear to the character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, false);
	NewSpear->AttachToComponent(SpearSpringArm, AttachmentRules);

	// Start the cooldown timer
	GetWorldTimerManager().SetTimer(ThrowCooldownTimerHandle, this, &ASpearCharacter::ResetThrowCooldown, CooldownDuration, false);
	bIsThrowOnCooldown = true;
}

void ASpearCharacter::ResetThrowCooldown()
{
	bIsThrowOnCooldown = false;
}

void ASpearCharacter::ThrowOngoing(const FInputActionValue& Value)
{

	if (!NewSpear)
	{
		UE_LOG(LogTemp, Warning, TEXT("NewSpear is null"));
		return;
	}

	if (!CameraBoom) 
	{
		UE_LOG(LogTemp, Warning, TEXT("CameraBoom is null"));
		return;
	}

	FVector SpearTargetLocation = GetSpearTargetLocation();

	float DeltaTime = GetWorld()->GetDeltaSeconds();
	float RotationLerpSpeed = 10.0f;

	FRotator CurrentRotation = NewSpear->GetActorRotation();
	FRotator NewRotation = (SpearTargetLocation - NewSpear->GetActorLocation()).Rotation();
	FRotator LerpedRotation = FMath::RInterpTo(CurrentRotation, NewRotation, DeltaTime, RotationLerpSpeed);

	// Clamp the pitch rotation to a range of -50 to 50 degrees
	LerpedRotation.Pitch = FMath::Clamp(LerpedRotation.Pitch, -40.0f, 50.0f);

	NewSpear->SetActorRotation(LerpedRotation);

	// Nudge Camera to Right
	FVector RightVector = CameraBoom->GetRightVector();
	FVector TargetOffset = RightVector * CameraNudgeMaxDistance;
	FVector CurrentOffset = CameraBoom->TargetOffset;

	// Interpolate the current offset towards the target offset
	float InterpolationAlpha = FMath::Clamp(GetWorld()->GetDeltaSeconds() * CameraNudgeSpeed, 0.0f, 1.0f);
	FVector NewOffset = FMath::VInterpTo(CurrentOffset, TargetOffset, InterpolationAlpha, CameraNudgeSpeed);

	CameraBoom->TargetOffset = NewOffset;
}

FVector ASpearCharacter::GetSpearTargetLocation()
{
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (!PlayerController)
	{
		return FVector::ZeroVector;
	}

	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	FVector2D ScreenCenter(ViewportSizeX * 0.5f, ViewportSizeY * 0.5f);

	FVector WorldLocation, WorldDirection;
	PlayerController->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.bTraceComplex = true;
	Params.AddIgnoredActor(this);
	if (NewSpear)
	{
		Params.AddIgnoredActor(NewSpear);
	}

	FVector SpearTargetLocation;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, WorldLocation + (WorldDirection * RaycastMaxDistance), ECC_Visibility, Params))
	{
		SpearTargetLocation = HitResult.Location;
	}
	else
	{
		SpearTargetLocation = WorldLocation + (WorldDirection * RaycastMaxDistance);
	}

	return SpearTargetLocation;
}



void ASpearCharacter::ThrowComplete(const FInputActionValue& Value)
{
	if (!NewSpear)
		return;

	// Detach the spear from the character
	NewSpear->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	NewSpear->bIsThrown = true;
	// Stop player from accessing once thrown
	NewSpear = nullptr;

	ECameraShakePlaySpace PlaySpace = ECameraShakePlaySpace::CameraLocal;
	APlayerCameraManager* CameraManager = Cast<APlayerController>(Controller)->PlayerCameraManager;
	CameraManager->StartCameraShake(SpearHoldingCameraShakeBlueprint, 1.0f, PlaySpace, FRotator(0.0f, 0.0f, 0.0f));

	MoveCameraBoomBack();

	// Add a debug message to the screen
	// FString DebugMessage = TEXT("Throw complete!");
	// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, DebugMessage);
}

void ASpearCharacter::MoveCameraBoomBack()
{
	if (NewSpear && CameraBoom)
	{
		float DeltaTime = GetWorld()->GetDeltaSeconds();

		FVector CurrentCameraLocation = CameraBoom->TargetOffset;
		FVector TargetCameraLocation = FVector::ZeroVector;

		// Calculate the movement direction
		FVector CameraMoveDirection = TargetCameraLocation - CurrentCameraLocation;
		CameraMoveDirection.Normalize();

		// Calculate the distance to move based on the camera move speed and delta time
		float DistanceToMove = CameraMoveBackSpeed * DeltaTime;

		// Move the camera towards the target location
		if ((TargetCameraLocation - CurrentCameraLocation).SizeSquared() > DistanceToMove * DistanceToMove)
		{
			FVector NewCameraLocation = CurrentCameraLocation + CameraMoveDirection * DistanceToMove;
			CameraBoom->TargetOffset = NewCameraLocation;
		}
		else
		{
			CameraBoom->TargetOffset = TargetCameraLocation;
		}

		// If the camera is not yet at the target location, mark the function to be called again in the next frame
		if ((TargetCameraLocation - CurrentCameraLocation).SizeSquared() > SMALL_NUMBER)
		{
			GetWorldTimerManager().SetTimerForNextTick([this] { MoveCameraBoomBack(); });
		}
	}
}

void ASpearCharacter::Recall(const FInputActionValue& Value)
{
	// Stop from recalling multiple spears at once
	if (bRecalling) {
		return;
	}

	// Get the forward vector of the character
	FVector ForwardVector = FollowCamera->GetForwardVector();

	// Calculate the end point of the raycast
	FVector CameraLocation = FollowCamera->GetComponentLocation();
	FVector EndPoint = CameraLocation + (ForwardVector * 5000);

	// Perform the raycast
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // Ignore the character itself

	if (GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, EndPoint, ECC_Visibility, QueryParams))
	{
		AActor* HitActor = HitResult.GetActor();
		if (!HitActor)
			return;

		if (!HitActor->IsA(ASpearActor::StaticClass()))
			return;

		ASpearActor* TempHitSpear = Cast<ASpearActor>(HitActor);
		if (!TempHitSpear)
			return;

		// Check if the spear is being recalled or is about to be destroyed
		if (TempHitSpear->bIsBeingRecalled || !TempHitSpear->IsValidLowLevel())
			return;

		HitSpear = TempHitSpear;
		HitSpear->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		SpearStartingDistance = FVector::Distance(GetActorLocation(), HitSpear->GetActorLocation());
		bRecalling = true;
	}

}

void ASpearCharacter::RecallTick(float DeltaTime)
{
	if (!bRecalling || !HitSpear)
		return;

	// Calculate the distance to the player
	float DistanceToPlayer = FVector::Distance(GetActorLocation(), HitSpear->GetActorLocation());

	// Check if the player is touching the hit spear
	bool bIsTouchingSpear = HitSpear->IsOverlappingActor(this);

	if (DistanceToPlayer <= SpearDeleteDistance || bIsTouchingSpear)
	{
		// Spear Death Particle Effects
		if (RecallPoofParticle)
		{
			FVector ParticleSpawnLocation = HitSpear->GetActorLocation();
			FRotator ParticleSpawnRotation = FRotator::ZeroRotator;
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), RecallPoofParticle, ParticleSpawnLocation, ParticleSpawnRotation);
		}
		HitSpear->DestroyWithEffects();

		// Delete the spear actor after a short delay
		// HitSpear->SetLifeSpan(DeleteDelay);
		HitSpear->bIsBeingRecalled = true;

		// Reset
		bRecalling = false;

		// End recall on spear delete
		HitSpear = nullptr;
		return;
	}

	// Scale down the spear as it gets close to the player
	float ScaleFactor = FMath::Clamp((DistanceToPlayer - SpearDeleteDistance) / (SpearStartingDistance - SpearDeleteDistance), 0.f, MaxScaleFactor);
	FVector NewScale = FVector::OneVector * ScaleFactor;
	HitSpear->SetActorScale3D(NewScale);

	// Move the spear towards the player in an arc
	FVector MoveDirection = GetActorLocation() - HitSpear->GetActorLocation();
	MoveDirection.Normalize();

	FVector MoveOffset = MoveDirection * MoveSpeed * DeltaTime;

	// Calculate the arc movement
	float DistanceFactor = FMath::Clamp(DistanceToPlayer / 100.f, 0.f, 1.f);
	float ArcOffsetZ = ArcHeight * FMath::Sin(DistanceFactor * PI);
	FVector ArcOffset = FVector(0.f, 0.f, ArcOffsetZ);
	FVector NewLocation = HitSpear->GetActorLocation() + MoveOffset + ArcOffset;

	HitSpear->SetActorLocation(NewLocation);
}


void ASpearCharacter::Swap(const FInputActionValue& Value)
{

}

void ASpearCharacter::PickedUpCollectible()
{
	if (PickUpCollectableParticle) {
		PickUpCollectableParticle->Activate();
	}

	CollectiblesPickedUp++;
}

