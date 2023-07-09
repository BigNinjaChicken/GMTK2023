// SequencePawn.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SequencePawn.generated.h"

UCLASS()
class GMTK2023_API ASequencePawn : public APawn
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class USceneComponent* RootSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* MappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* SkipSceneAction;

	UPROPERTY(EditAnywhere, Category = "Component")
	class UStaticMeshComponent* PageMesh;

	UPROPERTY(EditAnywhere, Category = "Component")
	class UStaticMeshComponent* BookMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	class UAudioComponent* FlipPageAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "G_Story")
	TArray<class UMaterial*> Pages;

public:
	// Sets default values for this pawn's properties
	ASequencePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SkipScene(const struct FInputActionValue& Value);

	void ShowNextPage();

	int PageIndex = 0;

	FName LevelName = FName("LV_Carrot");
};
