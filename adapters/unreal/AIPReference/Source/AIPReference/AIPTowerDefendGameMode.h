#pragma once

#include "CoreMinimal.h"
#include "AIPReferenceGameMode.h"
#include "AIPTowerDefendGameMode.generated.h"

/**
 * Default play mode: First Person pawn BPs + one-tower AIP arena.
 * World Settings on Lvl_FirstPerson may still use BP_FirstPersonGameMode;
 * that Blueprint subclasses AIPReferenceGameMode, which also EnsureArenaActors.
 */
UCLASS()
class AIPREFERENCE_API AAIPTowerDefendGameMode : public AAIPReferenceGameMode
{
	GENERATED_BODY()

public:
	AAIPTowerDefendGameMode();
};
