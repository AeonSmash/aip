#pragma once

#include "CoreMinimal.h"
#include "AIPWeapon.h"
#include "AIPLinkBeamWeapon.generated.h"

/** Short-range beam: LMB damages invaders, RMB repairs the home tower. */
UCLASS()
class AIPREFERENCE_API AAIPLinkBeamWeapon : public AAIPWeapon
{
	GENERATED_BODY()

public:
	AAIPLinkBeamWeapon();

	virtual void Tick(float DeltaTime) override;
	virtual FString GetWeaponDisplayName() const override { return TEXT("LinkBeam"); }

protected:
	void Pulse(bool bRepair);

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	float Range = 2200.f;

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	float DamagePerPulse = 12.f;

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	float RepairPerPulse = 28.f;

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	float PulseInterval = 0.1f;

	float PulseTimer = 0.f;
};
