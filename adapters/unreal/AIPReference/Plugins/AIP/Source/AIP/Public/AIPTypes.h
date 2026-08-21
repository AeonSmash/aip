#pragma once

#include "CoreMinimal.h"
#include "AIPTypes.generated.h"

USTRUCT(BlueprintType)
struct FAIPSource
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString World;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString App;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString Scene;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString IssuedAt;
};

USTRUCT(BlueprintType)
struct FAIPPowerContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString Scale;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString Tier;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString Basis;
};

USTRUCT(BlueprintType)
struct FAIPContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FAIPPowerContext Power;
};

USTRUCT(BlueprintType)
struct FAIPProvenance
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString Origin;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString DiscoveredBy;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	TArray<FString> History;
};

USTRUCT(BlueprintType)
struct FAIPRights
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString Owner;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	bool Transferable = false;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	bool Displayable = true;
};

USTRUCT(BlueprintType)
struct FAIPRepresentation
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString Glb;
};

/** AIP envelope 0.1 — every field that JSON fills MUST be UPROPERTY. */
USTRUCT(BlueprintType)
struct FAIPEnvelope
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString Aip;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString Kind;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString Id;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString Type;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString Label;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FAIPSource Source;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FAIPProvenance Provenance;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FAIPContext Context;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	TArray<FString> Capabilities;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FAIPRights Rights;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FAIPRepresentation Representation;
};

USTRUCT(BlueprintType)
struct FAIPMappedInterpretation
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString Destination;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString EnvelopeId;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString Principle;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString LocalType;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString LocalActor;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString Slot;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString Upgrade;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	float DamageMultiplier = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	TArray<FString> AcceptedCapabilities;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	TArray<FString> IgnoredCapabilities;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString Note;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString SourceWorld;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString SourceLabel;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString SourceType;

	UPROPERTY(BlueprintReadOnly, Category = "AIP")
	FString PowerTier;
};
