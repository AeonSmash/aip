// Copyright Epic Games, Inc. All Rights Reserved.

#include "AIPReferenceGameMode.h"
#include "AIPTerminal.h"
#include "Engine/World.h"
#include "EngineUtils.h"

AAIPReferenceGameMode::AAIPReferenceGameMode()
{
}

void AAIPReferenceGameMode::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	bool bFound = false;
	for (TActorIterator<AAIPTerminal> It(World); It; ++It)
	{
		bFound = true;
		break;
	}

	if (!bFound)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		World->SpawnActor<AAIPTerminal>(AAIPTerminal::StaticClass(), TerminalSpawnLocation, FRotator::ZeroRotator, Params);
		UE_LOG(LogTemp, Log, TEXT("AIP: spawned mid-field terminal at %s"), *TerminalSpawnLocation.ToString());
	}
}
