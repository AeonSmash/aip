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

	/** Barrel tip of the loaded viewmodel, in world space. */
	UFUNCTION(BlueprintPure, Category = "AIP|Weapon")
	FVector GetMuzzleWorldLocation() const;

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

	/** View space: X forward, Y right, Z up. */
	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	FVector ViewOffset = FVector(42.f, 20.f, -16.f);

	/** Optional tilt on top of the aim direction. Meshes are authored barrel-on-+X. */
	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	FRotator ViewRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Category = "AIP|Weapon")
	FVector ViewScale = FVector(0.35f, 0.12f, 0.12f);

	/** How far the viewmodel slides back on a shot, in cm. */
	UPROPERTY(EditAnywhere, Category = "AIP|Recoil")
	float RecoilKickBack = 5.f;

	/** How far the muzzle climbs on a shot, in degrees. */
	UPROPERTY(EditAnywhere, Category = "AIP|Recoil")
	float RecoilKickUp = 3.f;

	/** Higher settles back to rest faster. */
	UPROPERTY(EditAnywhere, Category = "AIP|Recoil")
	float RecoilRecovery = 9.f;

	void ApplyViewMesh();
	bool LoadObjViewMesh();

	/** Places the viewmodel from the player view point, in view space. */
	void UpdateViewTransform();

	/** Kicks the viewmodel back and up. Scale 1 is a full shot. */
	void AddRecoil(float Scale = 1.f);

	/** Barrel tip in viewmodel local space, measured when the mesh loads. */
	FVector MuzzleLocal = FVector::ZeroVector;

	/** 0 at rest, 1 just after a shot. */
	float RecoilAlpha = 0.f;

	bool bEquipped = true;
	bool bFiring = false;
	bool bAltFiring = false;
};
