#pragma once

#include "CoreMinimal.h"
#include "AIPWeapon.h"
#include "AIPLinkBeamWeapon.generated.h"

/** Discrete green energy spheres: LMB damages invaders, RMB repairs the home tower. */
UCLASS()
class AIPREFERENCE_API AAIPLinkBeamWeapon : public AAIPWeapon
{
	GENERATED_BODY()

public:
	AAIPLinkBeamWeapon();

	virtual void Tick(float DeltaTime) override;
	virtual void StartFire() override;
	virtual void StartAltFire() override;
	virtual FString GetWeaponDisplayName() const override { return TEXT("LinkBeam"); }

	UFUNCTION(BlueprintCallable, Category = "AIP|Weapon")
	void SetUnlocked(bool bInUnlocked);

	UFUNCTION(BlueprintPure, Category = "AIP|Weapon")
	bool IsUnlocked() const { return bUnlocked; }

protected:
	void TryLaunch(bool bRepair);

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	float Range = 2200.f;

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	float DamagePerShot = 40.f;

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	float RepairPerShot = 100.f;

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	float PulseInterval = 0.5f;

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	float MuzzleOffset = 88.f;

	float PulseTimer = 0.f;
	bool bUnlocked = false;
};
