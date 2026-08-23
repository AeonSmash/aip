// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AIPTypes.h"
#include "AIPReference.h"
#include "AIPReferenceCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UAIPPlayerUpgradeComponent;
class AAIPLinkBeamWeapon;
class AAIPStarterPistol;
class AAIPWeapon;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character with AIP upgrade + terminal interact keys.
 */
UCLASS(abstract)
class AAIPReferenceCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Destination-owned gun upgrade state from AIP envelopes. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AIP", meta = (AllowPrivateAccess = "true"))
	UAIPPlayerUpgradeComponent* AIPUpgrade;

protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* MouseLookAction;
	
public:
	AAIPReferenceCharacter();

protected:

	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	UFUNCTION()
	void OnAipInteract();

	UFUNCTION()
	void OnAipExport();

	UFUNCTION()
	void OnFirePressed();

	UFUNCTION()
	void OnFireReleased();

	UFUNCTION()
	void OnAltPressed();

	UFUNCTION()
	void OnAltReleased();

	UFUNCTION()
	void OnSelectPistol();

	UFUNCTION()
	void OnSelectLinkBeam();

	UFUNCTION()
	void OnNextWeapon();

	UFUNCTION()
	void OnPrevWeapon();

	UFUNCTION()
	void OnAipMappingApplied(const FAIPEnvelope& Envelope, const FAIPMappedInterpretation& Mapping);

	void SpawnArenaWeapons();
	void SuppressTemplatePistol();
	void EquipWeaponIndex(int32 Index);
	AAIPWeapon* GetEquippedWeapon() const;

	class AAIPTerminal* FindOverlappingTerminal() const;

	UPROPERTY()
	TObjectPtr<AAIPStarterPistol> StarterPistol;

	UPROPERTY()
	TObjectPtr<AAIPLinkBeamWeapon> LinkBeam;

	int32 EquippedWeaponIndex = 0;
	bool bLinkBeamUnlocked = false;
	float FootstepTimer = 0.f;

public:
	UFUNCTION(BlueprintPure, Category = "AIP|Weapon")
	FString GetEquippedWeaponName() const;

protected:

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	

public:

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UAIPPlayerUpgradeComponent* GetAIPUpgrade() const { return AIPUpgrade; }

};
