#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BreakableActor.generated.h"

UCLASS()
class GMTK2023_API ABreakableActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABreakableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* RootSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "G_Effects")
	class UNiagaraSystem* DestroyParticles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "G_Effects", meta = (MakeEditWidget = true))
	FVector ParticleSpawnLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "G_Audio")
	class USoundBase* BreakSound;

};
