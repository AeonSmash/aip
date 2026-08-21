// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AIPReferenceGameMode.generated.h"

/**
 * Concrete arena GameMode: UT-feel defaults + mid-field AIP terminal.
 */
UCLASS()
class AAIPReferenceGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAIPReferenceGameMode();

	virtual void BeginPlay() override;

	/** World location for the AIP terminal (mid-field). */
	UPROPERTY(EditAnywhere, Category = "AIP")
	FVector TerminalSpawnLocation = FVector(0.f, 800.f, 100.f);
};
