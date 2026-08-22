// Copyright Epic Games, Inc. All Rights Reserved.


#include "AIPReferencePlayerController.h"
#include "AIPSovereigntyWidget.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "AIPReferenceCameraManager.h"
#include "Blueprint/UserWidget.h"
#include "AIPReference.h"
#include "Widgets/Input/SVirtualJoystick.h"

AAIPReferencePlayerController::AAIPReferencePlayerController()
{
	PlayerCameraManagerClass = AAIPReferenceCameraManager::StaticClass();
}

void AAIPReferencePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController() && ShouldUseTouchControls())
	{
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);
		if (MobileControlsWidget)
		{
			MobileControlsWidget->AddToPlayerScreen(0);
		}
		else
		{
			UE_LOG(LogAIPReference, Error, TEXT("Could not spawn mobile controls widget."));
		}
	}

	if (IsLocalPlayerController())
	{
		SovereigntyWidget = CreateWidget<UAIPSovereigntyWidget>(this, UAIPSovereigntyWidget::StaticClass());
		if (SovereigntyWidget)
		{
			SovereigntyWidget->AddToViewport(10);
			SovereigntyWidget->SetSummary(TEXT("AIP: pull web switch, then E at terminal unlocks LinkBeam"));
			SovereigntyWidget->SetArenaStatus(TEXT("core -- | countdown | gun=Pistol"));
		}
	}
}

void AAIPReferencePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

void AAIPReferencePlayerController::SetArenaHud(const FString& Line)
{
	if (SovereigntyWidget)
	{
		SovereigntyWidget->SetArenaStatus(Line);
	}
}

bool AAIPReferencePlayerController::ShouldUseTouchControls() const
{
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}
