#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIPLinkSphereProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UPointLightComponent;

/** Green energy sphere from LinkBeam. Shrinks as it travels away from the muzzle. */
UCLASS()
class AIPREFERENCE_API AAIPLinkSphereProjectile : public AActor
{
	GENERATED_BODY()

public:
	AAIPLinkSphereProjectile();

	virtual void Tick(float DeltaTime) override;

	void InitShot(const FVector& Direction, bool bInRepair, float InDamage, float InRepair, AActor* InIgnore, float InRange);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIP|Weapon")
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIP|Weapon")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIP|Weapon")
	TObjectPtr<UProjectileMovementComponent> Movement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIP|Weapon")
	TObjectPtr<UPointLightComponent> Glow;

protected:
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void ApplyAndDestroy(AActor* HitActor);
	void RefreshScale();

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	float StartScale = 0.72f;

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	float EndScale = 0.14f;

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	float Speed = 2000.f;

	FVector MuzzleOrigin = FVector::ZeroVector;
	float MaxRange = 2200.f;
	float Damage = 40.f;
	float Repair = 100.f;
	bool bRepair = false;
	bool bConsumed = false;
	TWeakObjectPtr<AActor> IgnoredActor;
};
