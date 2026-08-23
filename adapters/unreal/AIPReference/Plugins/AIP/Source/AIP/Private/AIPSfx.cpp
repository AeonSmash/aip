#include "AIPSfx.h"

#include "Components/AudioComponent.h"
#include "HAL/FileManager.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Sound/SoundWaveProcedural.h"

namespace
{
	bool ParseWav(const TArray<uint8>& File, TArray<uint8>& OutPCM, int32& OutSampleRate, int32& OutChannels, float& OutDuration)
	{
		if (File.Num() < 44)
		{
			return false;
		}
		if (FMemory::Memcmp(File.GetData(), "RIFF", 4) != 0 || FMemory::Memcmp(File.GetData() + 8, "WAVE", 4) != 0)
		{
			return false;
		}

		int32 Offset = 12;
		int32 SampleRate = 0;
		int32 Channels = 0;
		int32 Bits = 0;
		const uint8* DataPtr = nullptr;
		int32 DataSize = 0;

		while (Offset + 8 <= File.Num())
		{
			const char* ChunkId = reinterpret_cast<const char*>(File.GetData() + Offset);
			const int32 ChunkSize = *reinterpret_cast<const int32*>(File.GetData() + Offset + 4);
			Offset += 8;
			if (Offset + ChunkSize > File.Num())
			{
				break;
			}

			if (FMemory::Memcmp(ChunkId, "fmt ", 4) == 0 && ChunkSize >= 16)
			{
				Channels = *reinterpret_cast<const int16*>(File.GetData() + Offset + 2);
				SampleRate = *reinterpret_cast<const int32*>(File.GetData() + Offset + 4);
				Bits = *reinterpret_cast<const int16*>(File.GetData() + Offset + 14);
			}
			else if (FMemory::Memcmp(ChunkId, "data", 4) == 0)
			{
				DataPtr = File.GetData() + Offset;
				DataSize = ChunkSize;
				break;
			}

			Offset += ChunkSize + (ChunkSize & 1);
		}

		if (!DataPtr || DataSize <= 0 || SampleRate <= 0 || Channels <= 0 || Bits != 16)
		{
			return false;
		}

		OutPCM.SetNumUninitialized(DataSize);
		FMemory::Memcpy(OutPCM.GetData(), DataPtr, DataSize);
		OutSampleRate = SampleRate;
		OutChannels = Channels;
		OutDuration = static_cast<float>(DataSize) / (2.f * Channels * SampleRate);
		return true;
	}
}

void UAIPSfxSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UAIPSfxSubsystem::Deinitialize()
{
	Cache.Empty();
	Super::Deinitialize();
}

UAIPSfxSubsystem* UAIPSfxSubsystem::Get(const UObject* WorldContext)
{
	const UWorld* World = WorldContext ? WorldContext->GetWorld() : nullptr;
	return World ? World->GetSubsystem<UAIPSfxSubsystem>() : nullptr;
}

bool UAIPSfxSubsystem::LoadCue(FName Cue, FCue& Out) const
{
	if (const FCue* Hit = Cache.Find(Cue))
	{
		Out = *Hit;
		return Out.bValid;
	}

	const FString Path = FPaths::ProjectContentDir() / TEXT("AIP/Audio") / Cue.ToString() + TEXT(".wav");
	TArray<uint8> File;
	FCue Loaded;
	if (FFileHelper::LoadFileToArray(File, *Path) && ParseWav(File, Loaded.PCM, Loaded.SampleRate, Loaded.Channels, Loaded.Duration))
	{
		Loaded.bValid = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AIP SFX missing or invalid: %s"), *Path);
	}

	const_cast<UAIPSfxSubsystem*>(this)->Cache.Add(Cue, Loaded);
	Out = Loaded;
	return Loaded.bValid;
}

USoundWaveProcedural* UAIPSfxSubsystem::MakeWave(const FCue& Cue, bool bLooping) const
{
	USoundWaveProcedural* Wave = NewObject<USoundWaveProcedural>();
	Wave->SetSampleRate(Cue.SampleRate);
	Wave->NumChannels = Cue.Channels;
	Wave->Duration = Cue.Duration;
	Wave->bLooping = bLooping;
	Wave->SoundGroup = SOUNDGROUP_Default;
	Wave->QueueAudio(Cue.PCM.GetData(), Cue.PCM.Num());
	return Wave;
}

void UAIPSfxSubsystem::Play(FName Cue, float Volume)
{
	FCue Loaded;
	if (!LoadCue(Cue, Loaded))
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	USoundWaveProcedural* Wave = MakeWave(Loaded, false);
	UGameplayStatics::PlaySound2D(World, Wave, FMath::Clamp(Volume, 0.f, 1.f));
}

void UAIPSfxSubsystem::PlayLoop(UAudioComponent* Component, FName Cue, float Volume)
{
	if (!Component)
	{
		return;
	}

	FCue Loaded;
	if (!LoadCue(Cue, Loaded))
	{
		return;
	}

	USoundWaveProcedural* Wave = MakeWave(Loaded, true);
	Component->SetSound(Wave);
	Component->SetVolumeMultiplier(FMath::Clamp(Volume, 0.f, 1.f));
	Component->bIsUISound = true;
	Component->Play();
}

void AIPSfx::Play(const UObject* WorldContext, FName Cue, float Volume)
{
	if (UAIPSfxSubsystem* Sfx = UAIPSfxSubsystem::Get(WorldContext))
	{
		Sfx->Play(Cue, Volume);
	}
}

void AIPSfx::PlayLoop(const UObject* WorldContext, UAudioComponent* Component, FName Cue, float Volume)
{
	if (UAIPSfxSubsystem* Sfx = UAIPSfxSubsystem::Get(WorldContext))
	{
		Sfx->PlayLoop(Component, Cue, Volume);
	}
}
