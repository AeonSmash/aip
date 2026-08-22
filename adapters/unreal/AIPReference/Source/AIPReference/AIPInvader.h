#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIPInvader.generated.h"

class AAIPCoreTower;
class UStaticMeshComponent;

/** Walks at the home tower and damages it in melee. Placeholder capsule + cube. */
UCLASS()
class AIPREFERENCE_API AAIPInvader : public ACharacter
{
	GENERATED_BODY()

public:
	AAIPInvader();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "AIP|Arena")
	void ReceiveWeaponDamage(float Amount, AActor* DamageCauser);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIP|Arena")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

protected:
	void Die();
	AAIPCoreTower* FindTower() const;

	UPROPERTY(EditAnywhere, Category = "AIP|Arena")
	float MaxHealth = 40.f;

	UPROPERTY(EditAnywhere, Category = "AIP|Arena")
	float MeleeDamage = 6.f;

	UPROPERTY(EditAnywhere, Category = "AIP|Arena")
	float MeleeInterval = 0.8f;

	UPROPERTY(EditAnywhere, Category = "AIP|Arena")
	float MeleeRange = 220.f;

	float Health = 40.f;
	float MeleeTimer = 0.f;
	TWeakObjectPtr<AAIPCoreTower> CachedTower;
};
