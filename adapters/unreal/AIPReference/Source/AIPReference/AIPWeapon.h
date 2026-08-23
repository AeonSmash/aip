#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/SoftObjectPath.h"
#include "AIPWeapon.generated.h"

class UStaticMeshComponent;
class UProceduralMeshComponent;
class ACharacter;

/**
 * First-person viewmodel. Prefers an imported static mesh, then a local OBJ
 * extracted from the Blender FBX. Aim traces from the camera, not the mesh.
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
	void AttachToOwnerCamera(class USceneComponent* Camera = nullptr);

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "AIP|Weapon")
	virtual FString GetWeaponDisplayName() const { return TEXT("Weapon"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIP|Weapon")
	TObjectPtr<UStaticMeshComponent> ViewMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AIP|Weapon")
	TObjectPtr<UProceduralMeshComponent> ProcMesh;

protected:
	bool GetAim(FVector& OutStart, FVector& OutEnd, float Range) const;

	ACharacter* GetPawnOwner() const;

	bool LineTrace(float Range, FHitResult& OutHit) const;

	void DrawBeam(const FVector& Start, const FVector& End, const FColor& Color, float Duration, float Thickness) const;

	/** Imported gun mesh. Empty keeps the placeholder cube. */
	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	FSoftObjectPath ViewMeshAsset;

	/** Content-relative OBJ extracted from the Blender FBX (used if the uasset is missing). */
	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	FString ViewObjFile;

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	FVector ViewOffset = FVector(42.f, 20.f, -16.f);

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	FRotator ViewRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	FVector ViewScale = FVector(0.35f, 0.12f, 0.12f);

	void ApplyViewMesh();
	bool LoadObjViewMesh();

	bool bEquipped = true;
	bool bFiring = false;
	bool bAltFiring = false;
};
