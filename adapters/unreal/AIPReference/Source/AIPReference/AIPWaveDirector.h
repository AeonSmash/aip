#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIPWaveDirector.generated.h"

class AAIPCoreTower;
class AAIPInvaderStart;

UENUM(BlueprintType)
enum class EAIPArenaState : uint8
{
	Countdown,
	Spawning,
	Resting,
	Victory,
	Defeat
};

/** Three short waves with a rest after wave 1 so the AIP terminal beat is playable. */
UCLASS()
class AIPREFERENCE_API AAIPWaveDirector : public AActor
{
	GENERATED_BODY()

public:
	AAIPWaveDirector();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "AIP|Arena")
	void NotifyInvaderKilled();

	UFUNCTION(BlueprintCallable, Category = "AIP|Arena")
	void NotifyTowerDestroyed();

	UFUNCTION(BlueprintPure, Category = "AIP|Arena")
	FString GetHudLine() const;

	UFUNCTION(BlueprintPure, Category = "AIP|Arena")
	EAIPArenaState GetArenaState() const { return State; }

protected:
	void StartWave(int32 WaveIndex);
	void SpawnOne();
	void RefreshHud();
	AAIPCoreTower* GetTower() const;
	FVector PickSpawnLocation() const;

	UPROPERTY(EditAnywhere, Category = "AIP|Arena")
	float InitialDelay = 5.f;

	UPROPERTY(EditAnywhere, Category = "AIP|Arena")
	float SpawnInterval = 1.2f;

	UPROPERTY(EditAnywhere, Category = "AIP|Arena")
	float RestAfterWave1 = 18.f;

	UPROPERTY(EditAnywhere, Category = "AIP|Arena")
	float RestAfterWave2 = 10.f;

	UPROPERTY(EditAnywhere, Category = "AIP|Arena")
	TArray<int32> WaveCounts;

	EAIPArenaState State = EAIPArenaState::Countdown;
	int32 CurrentWave = 0;
	int32 RemainingToSpawn = 0;
	int32 AliveInvaders = 0;
	float StateTimer = 0.f;
	FString LastHud;
};
