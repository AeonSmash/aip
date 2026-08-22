#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIPWeapon.generated.h"

class UStaticMeshComponent;
class ACharacter;

/**
 * First-person viewmodel placeholder. Swap the mesh later with a Blender FBX.
 * Aim traces from the owning pawn's camera, not the mesh.
 */
UCLASS(Abstract)
class AIPREFERENCE_API AAIPWeapon : public AActor
{
	GENERATED_BODY()

public:
	AAIPWeapon();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "AIP|Weapon")
	virtual void StartFire();

	UFUNCTION(BlueprintCallable, Category = "AIP|Weapon")
	virtual void StopFire();

	UFUNCTION(BlueprintCallable, Category = "AIP|Weapon")
	virtual void StartAltFire();

	UFUNCTION(BlueprintCallable, Category = "AIP|Weapon")
	virtual void StopAltFire();

	UFUNCTION(BlueprintCallable, Category = "AIP|Weapon")
	void SetEquipped(bool bInEquipped);

	UFUNCTION(BlueprintCallable, Category = "AIP|Weapon")
	void AttachToOwnerCamera();

	UFUNCTION(BlueprintPure, Category = "AIP|Weapon")
	virtual FString GetWeaponDisplayName() const { return TEXT("Weapon"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIP|Weapon")
	TObjectPtr<UStaticMeshComponent> ViewMesh;

protected:
	bool GetAim(FVector& OutStart, FVector& OutEnd, float Range) const;

	ACharacter* GetPawnOwner() const;

	bool LineTrace(float Range, FHitResult& OutHit) const;

	void DrawBeam(const FVector& Start, const FVector& End, const FColor& Color, float Duration, float Thickness) const;

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	FVector ViewOffset = FVector(42.f, 20.f, -16.f);

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	FVector ViewScale = FVector(0.35f, 0.12f, 0.12f);

	bool bEquipped = true;
	bool bFiring = false;
	bool bAltFiring = false;
};
