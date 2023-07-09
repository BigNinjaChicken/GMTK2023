// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollectibleActor.generated.h"

UCLASS()
class GMTK2023_API ACollectibleActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACollectibleActor();

	UPROPERTY(EditAnywhere, Category = "Component")
    class UStaticMeshComponent* CarrotMesh;

	UPROPERTY(EditAnywhere, Category = "G_Effects")
	class UNiagaraSystem* CarrotDeathParticle;

	UPROPERTY(EditAnywhere, Category = "G_Effects")
	class UNiagaraComponent* CarrotCryParticle;

	UPROPERTY(EditAnywhere, Category = "Component")
    class USphereComponent* CollisionSphere;

	UPROPERTY(EditAnywhere, Category = "Component")
	class UAudioComponent* ScaredAudio;

	UPROPERTY(EditAnywhere, Category = "Component")
	class UAudioComponent* BraveAudio;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnBeginOverlapMesh(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
    void OnBeginOverlapSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                        const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlapSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void PlayScaredAudio();
	void PlayBraveAudio();
	void LoopSound();
	void StopLoopedSound();

	FTimerHandle TimerHandle_StopSound;

	UPROPERTY(EditAnywhere, Category = "G_Sound")
	float ScaredLoopStartTime = 15.0f;

	UPROPERTY(EditAnywhere, Category = "G_Sound")
	float ScaredLoopEndTime = 17.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "G_Sound")
	bool bScaredLoopingSound = false;

	UPROPERTY(EditAnywhere, Category = "G_Sound")
	float BraveLoopStartTime = 15.0f;

	UPROPERTY(EditAnywhere, Category = "G_Sound")
	float BraveLoopEndTime = 17.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "G_Sound")
	bool bBraveLoopingSound = false;

	void DestroyWithEffects();

	UPROPERTY(EditAnywhere, Category = "G_Defensive")
	float RotateSpeed = 5.0f;
};
