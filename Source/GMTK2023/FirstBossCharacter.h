// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FirstBossCharacter.generated.h"

UENUM(BlueprintType)
enum class EBossFightPhase : uint8
{
    Phase1,
    Phase2,
	NotStarted
};

UCLASS()
class GMTK2023_API AFirstBossCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFirstBossCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CrystalMeshComponent;

	UPROPERTY(EditAnywhere, Category = "G_Effects")
	class UNiagaraComponent* CrystalEffectNiagaraComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Methods
	UFUNCTION()
	void OnCrystalMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void BeginPhase1();
	void BeginPhase2();
	void BeginPhase3();

	UFUNCTION(BlueprintImplementableEvent, Category = "G_Boss")
    void OnBossPhaseChanged(EBossFightPhase NewPhase);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBossFightPhase CurrentPhase = EBossFightPhase::NotStarted;
};
