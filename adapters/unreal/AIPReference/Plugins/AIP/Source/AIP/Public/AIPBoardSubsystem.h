#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AIPTypes.h"
#include "AIPBoardSubsystem.generated.h"

/**
 * Polls the AIP envelope board. Reveals the terminal on a live signal.box
 * (envelopes issued before this play session are ignored),
 * unlocks LinkBeam on signal.breaker. E posts signal.terminal.
 */
UCLASS()
class AIP_API UAIPBoardSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	UFUNCTION(BlueprintCallable, Category = "AIP")
	void NotifyTerminalActivated();

protected:
	void PollLatest();
	void HandleEnvelopes(const TArray<FAIPEnvelope>& Envelopes);
	void RevealTerminal();
	void ApplyBreaker(const FAIPEnvelope& Envelope);
	void PostTerminalEnvelope();
	void HandleLatestPayload(const FString& Json, int32 ResponseCode, bool bSucceeded);
	void HandlePostResult(int32 ResponseCode, bool bSucceeded);
	bool IsLiveForThisPlay(const FAIPEnvelope& Envelope) const;

	float PollTimer = 0.f;
	bool bTerminalRevealed = false;
	bool bBreakerApplied = false;
	bool bTerminalPosted = false;
	bool bPollInFlight = false;
	FDateTime PlayStartedUtc;
	TSet<FString> SeenEnvelopeIds;
};
