#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BrittleObjectComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GMTK2023_API UBrittleObjectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBrittleObjectComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool bIsBrittle = false;
	FTimerHandle ResetBrittleTimerHandle;
	FTimerHandle RestoreVisibilityTimerHandle;

	UFUNCTION()
	void OnSpearOverlap(AActor* OverlappedActor, AActor* OtherActor);

	void MakeInvisible();
	void MakeVisible();
	void TickAddBrittleTexture();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "G_Brittle")
	bool bIsInvisible;

	UPROPERTY(EditAnywhere, Category = "G_Brittle")
	float VisibleDuration = 5.0f;

	UPROPERTY(EditAnywhere, Category = "G_Brittle")
	float InvisibleDuration = 5.0f;

	FTimerHandle InvisibilityTimerHandle;

};
