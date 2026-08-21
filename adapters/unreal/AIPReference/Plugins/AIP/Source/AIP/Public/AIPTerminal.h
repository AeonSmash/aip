#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIPTypes.h"
#include "AIPTerminal.generated.h"

class UBoxComponent;
class UTextRenderComponent;
class UPrimitiveComponent;

/**
 * Mid-field terminal. Player presses Interact (E) while overlapping to load inbox envelope.
 * Press Export (F) to write a sigil to outbox.
 */
UCLASS()
class AIP_API AAIPTerminal : public AActor
{
	GENERATED_BODY()

public:
	AAIPTerminal();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "AIP")
	bool TryLoadInboxAndApply(APawn* InstigatorPawn, FString& OutStatus);

	UFUNCTION(BlueprintCallable, Category = "AIP")
	bool TryExportSigil(FString& OutStatus);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIP")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIP")
	TObjectPtr<UStaticMeshComponent> Pedestal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIP")
	TObjectPtr<UBoxComponent> InteractVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIP")
	TObjectPtr<UTextRenderComponent> PromptText;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FAIPEnvelope LastEnvelope;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FAIPMappedInterpretation LastMapping;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	bool bHasMapping = false;

protected:
	UFUNCTION()
	void HandleBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void HandleEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UPROPERTY()
	TWeakObjectPtr<APawn> OverlappingPawn;
};
