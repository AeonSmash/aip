#pragma once

#include "CoreMinimal.h"
#include "AIPWeapon.h"
#include "AIPStarterPistol.generated.h"

/** Weak starter pistol. Viewmodel is the lightning-gun FBX. Fires brass slugs. No core repair. */
UCLASS()
class AIPREFERENCE_API AAIPStarterPistol : public AAIPWeapon
{
	GENERATED_BODY()

public:
	AAIPStarterPistol();

	virtual void StartFire() override;
	virtual FString GetWeaponDisplayName() const override { return TEXT("Pistol"); }

protected:
	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	float Range = 2800.f;

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	float Damage = 8.f;

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	float Cooldown = 0.22f;

	float LastFireTime = -1000.f;
};
