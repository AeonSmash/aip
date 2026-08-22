#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIPCoreTower.generated.h"

class UStaticMeshComponent;
class UTextRenderComponent;

/** Single home core. Invaders damage it; LinkBeam alt-fire repairs it. */
UCLASS()
class AIPREFERENCE_API AAIPCoreTower : public AActor
{
	GENERATED_BODY()

public:
	AAIPCoreTower();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "AIP|Arena")
	void ReceiveInvaderDamage(float Amount);

	UFUNCTION(BlueprintCallable, Category = "AIP|Arena")
	void Repair(float Amount);

	UFUNCTION(BlueprintPure, Category = "AIP|Arena")
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintPure, Category = "AIP|Arena")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "AIP|Arena")
	bool IsDestroyedCore() const { return bDestroyed; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIP|Arena")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIP|Arena")
	TObjectPtr<UTextRenderComponent> HealthText;

protected:
	void RefreshLabel();

	UPROPERTY(EditAnywhere, Category = "AIP|Arena")
	float MaxHealth = 400.f;

	float Health = 400.f;
	bool bDestroyed = false;
};
