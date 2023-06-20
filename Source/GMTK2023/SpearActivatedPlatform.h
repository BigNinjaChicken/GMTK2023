#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpearActivatedPlatform.generated.h"

UCLASS()
class GMTK2023_API ASpearActivatedPlatform : public AActor
{
GENERATED_BODY()

public:
	ASpearActivatedPlatform();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* PlatformMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent* ParticleSystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	TArray<FVector> PlatformLocations;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MovementSpeed = 100.0f;

private:
	bool bIsMoving = false;
	int32 TargetLocationIndex = 0;

	bool IsTouchingSpearActor() const;
	void MoveToNextLocation(float DeltaTime);
	void StopMovement();
};

