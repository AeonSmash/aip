#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "AIPInvaderStart.generated.h"

/** Wave spawn marker. Place in the map or let GameMode spawn placeholders. */
UCLASS()
class AIPREFERENCE_API AAIPInvaderStart : public ATargetPoint
{
	GENERATED_BODY()

public:
	AAIPInvaderStart();
};
