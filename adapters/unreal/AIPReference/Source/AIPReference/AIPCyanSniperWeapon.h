#pragma once

#include "CoreMinimal.h"
#include "AIPWeapon.h"
#include "AIPCyanSniperWeapon.generated.h"

/** Slow cyan hitscan. Locked until an AIP envelope maps to unlock-sniper. */
UCLASS()
class AIPREFERENCE_API AAIPCyanSniperWeapon : public AAIPWeapon
{
	GENERATED_BODY()

public:
	AAIPCyanSniperWeapon();

	virtual void StartFire() override;
	virtual FString GetWeaponDisplayName() const override { return TEXT("CyanSniper"); }

	UFUNCTION(BlueprintCallable, Category = "AIP|Weapon")
	void SetUnlocked(bool bInUnlocked);

	UFUNCTION(BlueprintPure, Category = "AIP|Weapon")
	bool IsUnlocked() const { return bUnlocked; }

protected:
	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	float Range = 50000.f;

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	float Damage = 55.f;

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	float Cooldown = 1.0f;

	bool bUnlocked = false;
	float LastFireTime = -1000.f;
};
