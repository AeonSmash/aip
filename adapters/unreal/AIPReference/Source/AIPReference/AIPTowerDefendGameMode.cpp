#include "AIPTowerDefendGameMode.h"

#include "GameFramework/Pawn.h"
#include "UObject/ConstructorHelpers.h"

AAIPTowerDefendGameMode::AAIPTowerDefendGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PawnClass(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	if (PawnClass.Succeeded())
	{
		DefaultPawnClass = PawnClass.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PCClass(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonPlayerController"));
	if (PCClass.Succeeded())
	{
		PlayerControllerClass = PCClass.Class;
	}
}
