#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AIPTypes.h"
#include "AIPBlueprintLibrary.generated.h"

UCLASS()
class AIP_API UAIPBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Resolve aip/exchange on disk (walks up from the .uproject). */
	UFUNCTION(BlueprintCallable, Category = "AIP")
	static FString GetAIPExchangeRoot();

	UFUNCTION(BlueprintCallable, Category = "AIP")
	static FString GetAIPInboxDirectory();

	UFUNCTION(BlueprintCallable, Category = "AIP")
	static FString GetAIPOutboxDirectory();

	UFUNCTION(BlueprintCallable, Category = "AIP")
	static FString GetAIPMappingsDirectory();

	/**
	 * Load and parse an envelope JSON file.
	 * Does not use DirectoryWatcher — call from Interact input.
	 */
	UFUNCTION(BlueprintCallable, Category = "AIP")
	static bool LoadAipEnvelopeFromFile(const FString& AbsolutePath, FAIPEnvelope& OutEnvelope, FString& OutError);

	/** Load the first *.aip.json in exchange/inbox (or a preferred basename). */
	UFUNCTION(BlueprintCallable, Category = "AIP")
	static bool LoadAipEnvelopeFromInbox(FAIPEnvelope& OutEnvelope, FString& OutError, const FString& PreferredBaseName = TEXT(""));

	/** Apply mappings/unreal-fps.json rules (destination-owned data, not a C++ switch). */
	UFUNCTION(BlueprintCallable, Category = "AIP")
	static bool MapEnvelopeForUnrealFps(const FAIPEnvelope& Envelope, FAIPMappedInterpretation& OutMapped, FString& OutError);

	/** Write an achievement/sigil envelope to exchange/outbox. */
	UFUNCTION(BlueprintCallable, Category = "AIP")
	static bool ExportSigilToOutbox(const FString& Label, FString& OutWrittenPath, FString& OutError);

	UFUNCTION(BlueprintPure, Category = "AIP")
	static FString SanitizeEnvelopeIdForFilename(const FString& EnvelopeId);
};
