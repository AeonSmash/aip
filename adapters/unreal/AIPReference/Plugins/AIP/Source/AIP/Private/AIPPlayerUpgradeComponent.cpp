#include "AIPPlayerUpgradeComponent.h"

UAIPPlayerUpgradeComponent::UAIPPlayerUpgradeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAIPPlayerUpgradeComponent::ApplyMapping(const FAIPEnvelope& Envelope, const FAIPMappedInterpretation& Mapping)
{
	LastEnvelope = Envelope;
	LastMapping = Mapping;
	bHasUpgrade = Mapping.Upgrade == TEXT("tier1");
	DamageMultiplier = Mapping.DamageMultiplier > 0.f ? Mapping.DamageMultiplier : 1.f;
	OnMappingApplied.Broadcast(Envelope, Mapping);

	UE_LOG(LogTemp, Log, TEXT("AIP upgrade applied: %s -> %s (x%.2f) origin=%s"),
		*Mapping.SourceLabel, *Mapping.LocalType, DamageMultiplier, *Mapping.SourceWorld);
}

FString UAIPPlayerUpgradeComponent::GetHudSummary() const
{
	if (!bHasUpgrade && LastMapping.LocalType.IsEmpty())
	{
		return TEXT("AIP: no foreign envelope loaded");
	}

	return FString::Printf(
		TEXT("origin=%s | %s -> %s | upgrade=%s x%.2f | accept=[%s] ignore=[%s]"),
		*LastMapping.SourceWorld,
		*LastMapping.SourceType,
		*LastMapping.LocalType,
		*LastMapping.Upgrade,
		DamageMultiplier,
		*FString::Join(LastMapping.AcceptedCapabilities, TEXT(",")),
		*FString::Join(LastMapping.IgnoredCapabilities, TEXT(",")));
}
