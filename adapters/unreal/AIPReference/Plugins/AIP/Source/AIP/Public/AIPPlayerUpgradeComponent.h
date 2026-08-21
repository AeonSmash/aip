#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIPTypes.h"
#include "AIPPlayerUpgradeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAIPMappingApplied, const FAIPEnvelope&, Envelope, const FAIPMappedInterpretation&, Mapping);

/**
 * Holds destination-owned upgrade state for the local FPS pawn.
 */
UCLASS(ClassGroup = (AIP), meta = (BlueprintSpawnableComponent))
class AIP_API UAIPPlayerUpgradeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAIPPlayerUpgradeComponent();

	UFUNCTION(BlueprintCallable, Category = "AIP")
	void ApplyMapping(const FAIPEnvelope& Envelope, const FAIPMappedInterpretation& Mapping);

	UFUNCTION(BlueprintPure, Category = "AIP")
	float GetDamageMultiplier() const { return DamageMultiplier; }

	UFUNCTION(BlueprintPure, Category = "AIP")
	bool HasUpgrade() const { return bHasUpgrade; }

	UFUNCTION(BlueprintPure, Category = "AIP")
	FString GetHudSummary() const;

	UPROPERTY(BlueprintAssignable, Category = "AIP")
	FOnAIPMappingApplied OnMappingApplied;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FAIPEnvelope LastEnvelope;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FAIPMappedInterpretation LastMapping;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	bool bHasUpgrade = false;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	float DamageMultiplier = 1.0f;
};
