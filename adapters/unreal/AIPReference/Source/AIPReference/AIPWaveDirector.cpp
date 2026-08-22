#include "AIPWaveDirector.h"

#include "AIPCoreTower.h"
#include "AIPInvader.h"
#include "AIPInvaderStart.h"
#include "AIPReferenceCharacter.h"
#include "AIPReferencePlayerController.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

AAIPWaveDirector::AAIPWaveDirector()
{
	PrimaryActorTick.bCanEverTick = true;
	WaveCounts = {3, 5, 8};
}

void AAIPWaveDirector::BeginPlay()
{
	Super::BeginPlay();
	State = EAIPArenaState::Countdown;
	StateTimer = InitialDelay;
	CurrentWave = 0;
	RefreshHud();
}

void AAIPWaveDirector::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (State == EAIPArenaState::Defeat || State == EAIPArenaState::Victory)
	{
		RefreshHud();
		return;
	}

	StateTimer -= DeltaSeconds;

	if (State == EAIPArenaState::Countdown && StateTimer <= 0.f)
	{
		StartWave(1);
	}
	else if (State == EAIPArenaState::Spawning)
	{
		if (RemainingToSpawn > 0 && StateTimer <= 0.f)
		{
			SpawnOne();
			StateTimer = SpawnInterval;
		}

		if (RemainingToSpawn <= 0 && AliveInvaders <= 0)
		{
			if (CurrentWave >= WaveCounts.Num())
			{
				State = EAIPArenaState::Victory;
			}
			else
			{
				State = EAIPArenaState::Resting;
				StateTimer = (CurrentWave == 1) ? RestAfterWave1 : RestAfterWave2;
			}
		}
	}
	else if (State == EAIPArenaState::Resting && StateTimer <= 0.f)
	{
		StartWave(CurrentWave + 1);
	}

	RefreshHud();
}

void AAIPWaveDirector::StartWave(int32 WaveIndex)
{
	CurrentWave = WaveIndex;
	const int32 CountIndex = FMath::Clamp(WaveIndex - 1, 0, WaveCounts.Num() - 1);
	RemainingToSpawn = WaveCounts.IsValidIndex(CountIndex) ? WaveCounts[CountIndex] : 4;
	State = EAIPArenaState::Spawning;
	StateTimer = 0.f;
}

void AAIPWaveDirector::SpawnOne()
{
	UWorld* World = GetWorld();
	if (!World || RemainingToSpawn <= 0)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AAIPInvader* Invader = World->SpawnActor<AAIPInvader>(AAIPInvader::StaticClass(), PickSpawnLocation(), FRotator::ZeroRotator, Params);
	if (Invader)
	{
		++AliveInvaders;
		--RemainingToSpawn;
	}
}

void AAIPWaveDirector::NotifyInvaderKilled()
{
	AliveInvaders = FMath::Max(0, AliveInvaders - 1);
}

void AAIPWaveDirector::NotifyTowerDestroyed()
{
	State = EAIPArenaState::Defeat;
}

FVector AAIPWaveDirector::PickSpawnLocation() const
{
	UWorld* World = GetWorld();
	TArray<AAIPInvaderStart*> Starts;
	if (World)
	{
		for (TActorIterator<AAIPInvaderStart> It(World); It; ++It)
		{
			Starts.Add(*It);
		}
	}

	if (Starts.Num() > 0)
	{
		const int32 Index = FMath::RandRange(0, Starts.Num() - 1);
		return Starts[Index]->GetActorLocation();
	}

	if (const AAIPCoreTower* Tower = GetTower())
	{
		const FVector Offset = FVector(FMath::RandBool() ? 700.f : -700.f, FMath::RandRange(-200.f, 200.f), 0.f);
		return Tower->GetActorLocation() + Offset;
	}

	return FVector(700.f, 400.f, 100.f);
}

AAIPCoreTower* AAIPWaveDirector::GetTower() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}
	for (TActorIterator<AAIPCoreTower> It(World); It; ++It)
	{
		return *It;
	}
	return nullptr;
}

FString AAIPWaveDirector::GetHudLine() const
{
	const AAIPCoreTower* Tower = GetTower();
	const float HP = Tower ? Tower->GetHealth() : 0.f;
	const float MaxHP = Tower ? Tower->GetMaxHealth() : 0.f;

	FString Gun = TEXT("LinkBeam");
	if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		if (const AAIPReferenceCharacter* Character = Cast<AAIPReferenceCharacter>(Pawn))
		{
			Gun = Character->GetEquippedWeaponName();
		}
	}

	FString Phase;
	switch (State)
	{
	case EAIPArenaState::Countdown:
		Phase = FString::Printf(TEXT("wave 1 in %.0fs — repair core with RMB"), FMath::Max(0.f, StateTimer));
		break;
	case EAIPArenaState::Spawning:
		Phase = FString::Printf(TEXT("wave %d — left %d  alive %d"), CurrentWave, RemainingToSpawn, AliveInvaders);
		break;
	case EAIPArenaState::Resting:
		Phase = FString::Printf(TEXT("rest %.0fs — E at terminal unlocks CyanSniper"), FMath::Max(0.f, StateTimer));
		break;
	case EAIPArenaState::Victory:
		Phase = TEXT("VICTORY — F exports arena sigil");
		break;
	case EAIPArenaState::Defeat:
		Phase = TEXT("DEFEAT — core destroyed");
		break;
	}

	return FString::Printf(TEXT("core %.0f/%.0f | %s | gun=%s"), HP, MaxHP, *Phase, *Gun);
}

void AAIPWaveDirector::RefreshHud()
{
	const FString Line = GetHudLine();
	if (Line == LastHud)
	{
		return;
	}
	LastHud = Line;

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (AAIPReferencePlayerController* RefPC = Cast<AAIPReferencePlayerController>(PC))
		{
			RefPC->SetArenaHud(Line);
		}
	}
}
