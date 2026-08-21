// Copyright Epic Games, Inc. All Rights Reserved.

#include "AIPReferenceCharacter.h"
#include "AIPBlueprintLibrary.h"
#include "AIPPlayerUpgradeComponent.h"
#include "AIPSovereigntyWidget.h"
#include "AIPTerminal.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
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
