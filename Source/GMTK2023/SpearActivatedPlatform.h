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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "G_Movement", meta = (MakeEditWidget = true))
	TArray<FVector> PlatformLocations;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "G_Movement")
	float MovementSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "G_Movement")
	bool bIsAlwaysMoving = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "G_Movement")
	bool bOnlyMovesToFinalLocationOnce = false;


private:
	int32 TargetLocationIndex = 0;
	bool bCountingUp = true;

	bool IsTouchingSpearActor() const;
	void MoveToNextLocation(float DeltaTime);
};

