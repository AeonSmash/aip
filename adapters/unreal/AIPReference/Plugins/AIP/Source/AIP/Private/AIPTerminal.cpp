#include "AIPTerminal.h"

#include "AIPBlueprintLibrary.h"
#include "AIPPlayerUpgradeComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Pawn.h"
#include "UObject/ConstructorHelpers.h"

AAIPTerminal::AAIPTerminal()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Pedestal = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pedestal"));
	Pedestal->SetupAttachment(Root);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Pedestal->SetStaticMesh(CubeMesh.Object);
	}
	Pedestal->SetRelativeScale3D(FVector(1.2f, 1.2f, 2.0f));
	Pedestal->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	Pedestal->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	InteractVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractVolume"));
	InteractVolume->SetupAttachment(Root);
	InteractVolume->SetBoxExtent(FVector(180.f, 180.f, 160.f));
	InteractVolume->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	InteractVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractVolume->SetGenerateOverlapEvents(true);

	PromptText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PromptText"));
	PromptText->SetupAttachment(Root);
	PromptText->SetRelativeLocation(FVector(0.f, 0.f, 240.f));
	PromptText->SetHorizontalAlignment(EHTA_Center);
	PromptText->SetText(FText::FromString(TEXT("AIP Terminal\n[E] Signal other worlds\n[F] Export sigil")));
	PromptText->SetWorldSize(28.f);
	bRevealed = false;
}

void AAIPTerminal::BeginPlay()
{
	Super::BeginPlay();
	InteractVolume->OnComponentBeginOverlap.AddDynamic(this, &AAIPTerminal::HandleBeginOverlap);
	InteractVolume->OnComponentEndOverlap.AddDynamic(this, &AAIPTerminal::HandleEndOverlap);
	SetRevealed(bRevealed);
}

void AAIPTerminal::SetRevealed(bool bInRevealed)
{
	bRevealed = bInRevealed;
	SetActorHiddenInGame(!bRevealed);
	SetActorEnableCollision(bRevealed);
	if (PromptText)
	{
		PromptText->SetHiddenInGame(!bRevealed);
	}
}

void AAIPTerminal::HandleBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		OverlappingPawn = Pawn;
	}
}

void AAIPTerminal::HandleEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OverlappingPawn.Get() == OtherActor)
	{
		OverlappingPawn = nullptr;
	}
}

bool AAIPTerminal::TryLoadInboxAndApply(APawn* InstigatorPawn, FString& OutStatus)
{
	APawn* Pawn = InstigatorPawn ? InstigatorPawn : OverlappingPawn.Get();
	if (!Pawn)
	{
		OutStatus = TEXT("No overlapping pawn.");
		return false;
	}

	FString Error;
	if (!UAIPBlueprintLibrary::LoadAipEnvelopeFromInbox(LastEnvelope, Error))
	{
		OutStatus = Error;
		UE_LOG(LogTemp, Warning, TEXT("AIP Terminal load failed: %s"), *Error);
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("AIP loaded label=%s type=%s tier=%s"),
		*LastEnvelope.Label, *LastEnvelope.Type, *LastEnvelope.Context.Power.Tier);

	if (!UAIPBlueprintLibrary::MapEnvelopeForUnrealFps(LastEnvelope, LastMapping, Error))
	{
		bHasMapping = false;
		OutStatus = Error;
		UE_LOG(LogTemp, Warning, TEXT("AIP map failed: %s"), *Error);
		return false;
	}

	bHasMapping = true;
	if (UAIPPlayerUpgradeComponent* Upgrade = Pawn->FindComponentByClass<UAIPPlayerUpgradeComponent>())
	{
		Upgrade->ApplyMapping(LastEnvelope, LastMapping);
	}

	OutStatus = FString::Printf(
		TEXT("Mapped %s (%s) -> %s upgrade=%s x%.2f | accepted=%s ignored=%s"),
		*LastMapping.SourceLabel,
		*LastMapping.SourceWorld,
		*LastMapping.LocalType,
		*LastMapping.Upgrade,
		LastMapping.DamageMultiplier,
		*FString::Join(LastMapping.AcceptedCapabilities, TEXT(",")),
		*FString::Join(LastMapping.IgnoredCapabilities, TEXT(",")));

	PromptText->SetText(FText::FromString(OutStatus));
	return true;
}

bool AAIPTerminal::TryExportSigil(FString& OutStatus)
{
	FString Path;
	FString Error;
	if (!UAIPBlueprintLibrary::ExportSigilToOutbox(TEXT("Arena Sigil"), Path, Error))
	{
		OutStatus = Error;
		return false;
	}
	OutStatus = FString::Printf(TEXT("Wrote %s"), *Path);
	PromptText->SetText(FText::FromString(OutStatus));
	return true;
}
