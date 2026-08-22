// Copyright Epic Games, Inc. All Rights Reserved.

#include "AIPReferenceCharacter.h"
#include "AIPBlueprintLibrary.h"
#include "AIPPlayerUpgradeComponent.h"
#include "AIPSovereigntyWidget.h"
#include "AIPTerminal.h"
#include "AIPLinkBeamWeapon.h"
#include "AIPCyanSniperWeapon.h"
#include "AIPWeapon.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputCoreTypes.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "UObject/UObjectIterator.h"
#include "AIPReference.h"

AAIPReferenceCharacter::AAIPReferenceCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));
	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	// UT2004-inspired wider FOV
	FirstPersonCameraComponent->FirstPersonFieldOfView = 100.0f;
	FirstPersonCameraComponent->FieldOfView = 100.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	AIPUpgrade = CreateDefaultSubobject<UAIPPlayerUpgradeComponent>(TEXT("AIPUpgrade"));

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Faster arena pacing
	GetCharacterMovement()->MaxWalkSpeed = 900.f;
	GetCharacterMovement()->MaxAcceleration = 4096.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2048.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
	GetCharacterMovement()->JumpZVelocity = 700.f;
}

void AAIPReferenceCharacter::BeginPlay()
{
	Super::BeginPlay();
	SuppressTemplatePistol();
	SpawnArenaWeapons();
	if (AIPUpgrade)
	{
		AIPUpgrade->OnMappingApplied.AddDynamic(this, &AAIPReferenceCharacter::OnAipMappingApplied);
	}
}

void AAIPReferenceCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AAIPReferenceCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AAIPReferenceCharacter::DoJumpEnd);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAIPReferenceCharacter::MoveInput);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAIPReferenceCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AAIPReferenceCharacter::LookInput);
	}
	else
	{
		UE_LOG(LogAIPReference, Error, TEXT("'%s' Failed to find an Enhanced Input Component!"), *GetNameSafe(this));
	}

	// AIP terminal keys (input action assets not required)
	PlayerInputComponent->BindKey(EKeys::E, IE_Pressed, this, &AAIPReferenceCharacter::OnAipInteract);
	PlayerInputComponent->BindKey(EKeys::F, IE_Pressed, this, &AAIPReferenceCharacter::OnAipExport);
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AAIPReferenceCharacter::OnFirePressed);
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &AAIPReferenceCharacter::OnFireReleased);
	PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &AAIPReferenceCharacter::OnAltPressed);
	PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Released, this, &AAIPReferenceCharacter::OnAltReleased);
	PlayerInputComponent->BindKey(EKeys::One, IE_Pressed, this, &AAIPReferenceCharacter::OnSelectLinkBeam);
	PlayerInputComponent->BindKey(EKeys::Two, IE_Pressed, this, &AAIPReferenceCharacter::OnSelectSniper);
	PlayerInputComponent->BindKey(EKeys::MouseScrollUp, IE_Pressed, this, &AAIPReferenceCharacter::OnNextWeapon);
	PlayerInputComponent->BindKey(EKeys::MouseScrollDown, IE_Pressed, this, &AAIPReferenceCharacter::OnPrevWeapon);
}

AAIPTerminal* AAIPReferenceCharacter::FindOverlappingTerminal() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	AAIPTerminal* Best = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();
	const FVector Loc = GetActorLocation();
	for (TActorIterator<AAIPTerminal> It(World); It; ++It)
	{
		const float DistSq = FVector::DistSquared(Loc, It->GetActorLocation());
		if (DistSq < 250.f * 250.f && DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			Best = *It;
		}
	}
	return Best;
}

void AAIPReferenceCharacter::OnAipInteract()
{
	AAIPTerminal* Terminal = FindOverlappingTerminal();
	if (!Terminal)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIP: no terminal nearby (stand next to the mid-field cube)."));
		return;
	}

	FString Status;
	const bool bOk = Terminal->TryLoadInboxAndApply(this, Status);
	UE_LOG(LogTemp, Log, TEXT("AIP Interact: %s"), *Status);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		for (TObjectIterator<UAIPSovereigntyWidget> It; It; ++It)
		{
			if (It->GetOwningPlayer() == PC)
			{
				It->SetSummary(AIPUpgrade ? AIPUpgrade->GetHudSummary() : Status);
			}
		}
	}

	if (bOk && AIPUpgrade)
	{
		UE_LOG(LogTemp, Log, TEXT("AIP HUD: %s"), *AIPUpgrade->GetHudSummary());
	}
}

void AAIPReferenceCharacter::SpawnArenaWeapons()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	LinkBeam = World->SpawnActor<AAIPLinkBeamWeapon>(AAIPLinkBeamWeapon::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, Params);
	CyanSniper = World->SpawnActor<AAIPCyanSniperWeapon>(AAIPCyanSniperWeapon::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, Params);

	if (LinkBeam)
	{
		LinkBeam->AttachToOwnerCamera();
	}
	if (CyanSniper)
	{
		CyanSniper->AttachToOwnerCamera();
		CyanSniper->SetUnlocked(false);
	}
	EquipWeaponIndex(0);
}

void AAIPReferenceCharacter::SuppressTemplatePistol()
{
	TArray<USkeletalMeshComponent*> Meshes;
	GetComponents<USkeletalMeshComponent>(Meshes);
	for (USkeletalMeshComponent* MeshComp : Meshes)
	{
		if (MeshComp && MeshComp != FirstPersonMesh && MeshComp != GetMesh())
		{
			MeshComp->SetHiddenInGame(true);
			MeshComp->SetComponentTickEnabled(false);
			MeshComp->SetCastShadow(false);
			MeshComp->SetVisibility(false, true);
		}
	}

	TArray<AActor*> Attached;
	GetAttachedActors(Attached, true, true);
	for (AActor* Child : Attached)
	{
		if (!Child || Child->IsA<AAIPWeapon>())
		{
			continue;
		}
		const FString Name = Child->GetClass()->GetName();
		if (Name.Contains(TEXT("Weapon")) || Name.Contains(TEXT("Rifle")) || Name.Contains(TEXT("Pistol")) || Name.Contains(TEXT("Gun")))
		{
			Child->SetActorHiddenInGame(true);
			Child->SetActorEnableCollision(false);
			Child->SetActorTickEnabled(false);
		}
	}
}

void AAIPReferenceCharacter::EquipWeaponIndex(int32 Index)
{
	if (Index == 1 && !bSniperUnlocked)
	{
		Index = 0;
	}

	EquippedWeaponIndex = Index;
	if (LinkBeam)
	{
		LinkBeam->SetEquipped(Index == 0);
	}
	if (CyanSniper)
	{
		CyanSniper->SetEquipped(Index == 1 && bSniperUnlocked);
	}
}

AAIPWeapon* AAIPReferenceCharacter::GetEquippedWeapon() const
{
	if (EquippedWeaponIndex == 1)
	{
		return CyanSniper;
	}
	return LinkBeam;
}

FString AAIPReferenceCharacter::GetEquippedWeaponName() const
{
	if (const AAIPWeapon* Weapon = GetEquippedWeapon())
	{
		return Weapon->GetWeaponDisplayName();
	}
	return TEXT("LinkBeam");
}

void AAIPReferenceCharacter::OnFirePressed()
{
	if (AAIPWeapon* Weapon = GetEquippedWeapon())
	{
		Weapon->StartFire();
	}
}

void AAIPReferenceCharacter::OnFireReleased()
{
	if (AAIPWeapon* Weapon = GetEquippedWeapon())
	{
		Weapon->StopFire();
	}
}

void AAIPReferenceCharacter::OnAltPressed()
{
	if (AAIPWeapon* Weapon = GetEquippedWeapon())
	{
		Weapon->StartAltFire();
	}
}

void AAIPReferenceCharacter::OnAltReleased()
{
	if (AAIPWeapon* Weapon = GetEquippedWeapon())
	{
		Weapon->StopAltFire();
	}
}

void AAIPReferenceCharacter::OnSelectLinkBeam()
{
	EquipWeaponIndex(0);
}

void AAIPReferenceCharacter::OnSelectSniper()
{
	if (bSniperUnlocked)
	{
		EquipWeaponIndex(1);
	}
}

void AAIPReferenceCharacter::OnNextWeapon()
{
	EquipWeaponIndex(bSniperUnlocked && EquippedWeaponIndex == 0 ? 1 : 0);
}

void AAIPReferenceCharacter::OnPrevWeapon()
{
	OnNextWeapon();
}

void AAIPReferenceCharacter::OnAipMappingApplied(const FAIPEnvelope& Envelope, const FAIPMappedInterpretation& Mapping)
{
	if (AIPUpgrade && AIPUpgrade->HasSniperUnlock())
	{
		bSniperUnlocked = true;
		if (CyanSniper)
		{
			CyanSniper->SetUnlocked(true);
		}
		UE_LOG(LogAIPReference, Log, TEXT("AIP unlocked CyanSniper from %s"), *Envelope.Label);
	}
}

void AAIPReferenceCharacter::OnAipExport()
{
	AAIPTerminal* Terminal = FindOverlappingTerminal();
	if (!Terminal)
	{
		// Allow export from anywhere for the outbound demo
		FString Path;
		FString Error;
		if (UAIPBlueprintLibrary::ExportSigilToOutbox(TEXT("Arena Sigil"), Path, Error))
		{
			UE_LOG(LogTemp, Log, TEXT("AIP Export: %s"), *Path);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AIP Export failed: %s"), *Error);
		}
		return;
	}

	FString Status;
	Terminal->TryExportSigil(Status);
	UE_LOG(LogTemp, Log, TEXT("AIP Export: %s"), *Status);
}

void AAIPReferenceCharacter::MoveInput(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	DoMove(MovementVector.X, MovementVector.Y);
}

void AAIPReferenceCharacter::LookInput(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	DoAim(LookAxisVector.X, LookAxisVector.Y);
}

void AAIPReferenceCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AAIPReferenceCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void AAIPReferenceCharacter::DoJumpStart()
{
	Jump();
}

void AAIPReferenceCharacter::DoJumpEnd()
{
	StopJumping();
}
