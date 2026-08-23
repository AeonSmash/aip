// Copyright Epic Games, Inc. All Rights Reserved.

#include "AIPReferenceGameMode.h"
#include "AIPCoreTower.h"
#include "AIPInvaderStart.h"
#include "AIPTerminal.h"
#include "AIPWaveDirector.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "EngineUtils.h"

AAIPReferenceGameMode::AAIPReferenceGameMode()
{
}

void AAIPReferenceGameMode::BeginPlay()
{
	Super::BeginPlay();
	EnsureArenaActors();
}

void AAIPReferenceGameMode::EnsureArenaActors()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// FBX import can drop a world-space gun into the map. Hide it so only the viewmodel shows.
	for (TActorIterator<AStaticMeshActor> It(World); It; ++It)
	{
		UStaticMeshComponent* MeshComp = It->GetStaticMeshComponent();
		const UStaticMesh* Mesh = MeshComp ? MeshComp->GetStaticMesh() : nullptr;
		if (!Mesh)
		{
			continue;
		}
		const FString Name = Mesh->GetName();
		if (Name.Contains(TEXT("LightningGun")) || Name.Contains(TEXT("LinkGun")) || Name.Contains(TEXT("SNIPERriffle")) || Name.Contains(TEXT("LINKgun")))
		{
			It->Destroy();
			UE_LOG(LogTemp, Log, TEXT("AIP: removed stray world gun %s"), *Name);
		}
	}

	bool bTerminal = false;
	for (TActorIterator<AAIPTerminal> It(World); It; ++It)
	{
		bTerminal = true;
		It->SetRevealed(false);
		break;
	}
	if (!bTerminal)
	{
		World->SpawnActor<AAIPTerminal>(AAIPTerminal::StaticClass(), TerminalSpawnLocation, FRotator::ZeroRotator, Params);
		UE_LOG(LogTemp, Log, TEXT("AIP: spawned hidden terminal at %s"), *TerminalSpawnLocation.ToString());
		for (TActorIterator<AAIPTerminal> It(World); It; ++It)
		{
			It->SetRevealed(false);
			break;
		}
	}

	bool bTower = false;
	for (TActorIterator<AAIPCoreTower> It(World); It; ++It)
	{
		bTower = true;
		break;
	}
	if (!bTower)
	{
		World->SpawnActor<AAIPCoreTower>(AAIPCoreTower::StaticClass(), TowerSpawnLocation, FRotator::ZeroRotator, Params);
		UE_LOG(LogTemp, Log, TEXT("AIP: spawned core tower at %s"), *TowerSpawnLocation.ToString());
	}

	int32 StartCount = 0;
	for (TActorIterator<AAIPInvaderStart> It(World); It; ++It)
	{
		++StartCount;
	}
	if (StartCount == 0)
	{
		const FVector A = TowerSpawnLocation + FVector(700.f, 0.f, 20.f);
		const FVector B = TowerSpawnLocation + FVector(-700.f, 0.f, 20.f);
		World->SpawnActor<AAIPInvaderStart>(AAIPInvaderStart::StaticClass(), A, FRotator::ZeroRotator, Params);
		World->SpawnActor<AAIPInvaderStart>(AAIPInvaderStart::StaticClass(), B, FRotator::ZeroRotator, Params);
		UE_LOG(LogTemp, Log, TEXT("AIP: spawned invader start points"));
	}

	bool bDirector = false;
	for (TActorIterator<AAIPWaveDirector> It(World); It; ++It)
	{
		bDirector = true;
		break;
	}
	if (!bDirector)
	{
		World->SpawnActor<AAIPWaveDirector>(AAIPWaveDirector::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, Params);
	}
}
