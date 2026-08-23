#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIPPistolSlugProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UPointLightComponent;

/** Small brass slug from the starter pistol. Faster and thinner than LinkBeam orbs. */
UCLASS()
class AIPREFERENCE_API AAIPPistolSlugProjectile : public AActor
{
	GENERATED_BODY()

public:
	AAIPPistolSlugProjectile();

	virtual void Tick(float DeltaTime) override;

	void InitShot(const FVector& Direction, float InDamage, AActor* InIgnore, float InRange);

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
	float StartScale = 0.22f;

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	float EndScale = 0.12f;

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	float Speed = 5600.f;

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	float GravityScale = 0.35f;

	FVector MuzzleOrigin = FVector::ZeroVector;
	float MaxRange = 2800.f;
	float Damage = 8.f;
	bool bConsumed = false;
	TWeakObjectPtr<AActor> IgnoredActor;
};
