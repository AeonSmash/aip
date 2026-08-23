#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AIPSfx.generated.h"

class USoundWave;
class USoundWaveProcedural;
class UAudioComponent;

/** Loads Content/AIP/Audio/*.wav at runtime and plays 2D cues. */
UCLASS()
class AIP_API UAIPSfxSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }

	UFUNCTION(BlueprintCallable, Category = "AIP|Audio")
	void Play(FName Cue, float Volume = 1.f);

	UFUNCTION(BlueprintCallable, Category = "AIP|Audio")
	void PlayLoop(UAudioComponent* Component, FName Cue, float Volume = 1.f);

	static UAIPSfxSubsystem* Get(const UObject* WorldContext);

protected:
	struct FCue
	{
		TArray<uint8> PCM;
		int32 SampleRate = 24000;
		int32 Channels = 1;
		float Duration = 0.f;
		bool bValid = false;
	};

	bool LoadCue(FName Cue, FCue& Out) const;
	USoundWaveProcedural* MakeWave(const FCue& Cue, bool bLooping) const;

	TMap<FName, FCue> Cache;
};

namespace AIPSfx
{
	AIP_API void Play(const UObject* WorldContext, FName Cue, float Volume = 1.f);
	AIP_API void PlayLoop(const UObject* WorldContext, UAudioComponent* Component, FName Cue, float Volume = 1.f);
}
