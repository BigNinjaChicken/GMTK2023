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

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ASpearCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Switch input mode back to game input
	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);

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

		//Thowing
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Started, this, &ASpearCharacter::Throw);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Ongoing, this, &ASpearCharacter::ThrowOngoing);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Completed, this, &ASpearCharacter::ThrowComplete);
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
	FVector SpawnLocation = GetActorLocation() + SpearSpawnOffset; // Adjust the Z-axis offset as needed

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	FVector2D ScreenCenter(ViewportSizeX * 0.5f, ViewportSizeY * 0.5f);

	FVector WorldLocation, WorldDirection;
	PlayerController->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.bTraceComplex = true;
	Params.AddIgnoredActor(this);

	FVector SpearTargetLocation;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, WorldLocation + (WorldDirection * RaycastMaxDistance), ECC_Visibility, Params))
	{
		SpearTargetLocation = HitResult.Location;
	}
	else
	{
		SpearTargetLocation = WorldLocation + (WorldDirection * RaycastMaxDistance);
	}

	float DeltaTime = GetWorld()->GetDeltaSeconds();

	// Nudge Camera to Right
	FVector CameraBoomLocation = CameraBoom->GetComponentLocation();
	FVector RightVector = CameraBoom->GetRightVector();
	CameraBoom->TargetOffset = FMath::Lerp(CameraBoom->TargetOffset, CameraNudgeMaxDistance, DeltaTime * CameraNudgeSpeed);

	// Calculate rotation interpolation
	float RotationLerpSpeed = 100.0f;
	FRotator SpawnRotation = (SpearTargetLocation - SpawnLocation).Rotation();
	NewSpear = GetWorld()->SpawnActor<ASpearActor>(SpearActorBlueprint, SpawnLocation, SpawnRotation);

	if (NewSpear)
	{
		// Attach the spear to the character
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, false);
		NewSpear->AttachToComponent(SpearSpringArm, AttachmentRules);
	}
}

void ASpearCharacter::ThrowOngoing(const FInputActionValue& Value)
{
	if (NewSpear && CameraBoom)
	{
		APlayerController* PlayerController = Cast<APlayerController>(Controller);
		int32 ViewportSizeX, ViewportSizeY;
		PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

		FVector2D ScreenCenter(ViewportSizeX * 0.5f, ViewportSizeY * 0.5f);

		FVector WorldLocation, WorldDirection;
		PlayerController->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection);

		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.bTraceComplex = true;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(NewSpear);

		FVector SpearTargetLocation;
		if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, WorldLocation + (WorldDirection * 10000.0f), ECC_Visibility, Params))
		{
			SpearTargetLocation = HitResult.Location;
		}
		else
		{
			SpearTargetLocation = WorldLocation + (WorldDirection * 10000.0f);
		}

		float DeltaTime = GetWorld()->GetDeltaSeconds();

		// Nudge Camera to Right
		FVector CameraBoomLocation = CameraBoom->GetComponentLocation();
		FVector RightVector = CameraBoom->GetRightVector();
		CameraBoom->TargetOffset = FMath::Lerp(CameraBoom->TargetOffset, CameraNudgeMaxDistance, DeltaTime * CameraNudgeSpeed);

		// Calculate rotation interpolation
		float RotationLerpSpeed = 100.0f;
		FRotator CurrentRotation = NewSpear->GetActorRotation();
		FRotator NewRotation = (SpearTargetLocation - NewSpear->GetActorLocation()).Rotation();
		FRotator LerpedRotation = FMath::Lerp(CurrentRotation, NewRotation, DeltaTime * RotationLerpSpeed);

		// Clamp the pitch rotation to a range of -50 to 50 degrees
		LerpedRotation.Pitch = FMath::Clamp(LerpedRotation.Pitch, -50.0f, 50.0f);

		NewSpear->SetActorRotation(LerpedRotation);
	}
}



void ASpearCharacter::ThrowComplete(const FInputActionValue& Value)
{
	// Detach the spear from the character
	NewSpear->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	NewSpear->bIsThrown = true;

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
		float DistanceToMove = CameraMoveSpeed * DeltaTime;

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
