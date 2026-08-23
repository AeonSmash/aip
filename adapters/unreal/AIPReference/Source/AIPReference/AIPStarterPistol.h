#pragma once

#include "CoreMinimal.h"
#include "AIPWeapon.h"
#include "AIPStarterPistol.generated.h"

/** Weak starter pistol. Fires small brass slugs. No core repair. */
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

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	float MuzzleOffset = 70.f;

	float LastFireTime = -1000.f;
};
