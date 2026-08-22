#include "AIPCoreTower.h"

#include "AIPWaveDirector.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMesh.h"
#include "EngineUtils.h"
#include "UObject/ConstructorHelpers.h"

AAIPCoreTower::AAIPCoreTower()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetRelativeScale3D(FVector(2.2f, 2.2f, 6.0f));
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMesh.Object);
	}

	HealthText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HealthText"));
	HealthText->SetupAttachment(RootComponent);
	HealthText->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	HealthText->SetHorizontalAlignment(EHTA_Center);
	HealthText->SetWorldSize(28.f);
	HealthText->SetTextRenderColor(FColor(220, 230, 255));
}

void AAIPCoreTower::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
	RefreshLabel();
}

void AAIPCoreTower::ReceiveInvaderDamage(float Amount)
{
	if (bDestroyed || Amount <= 0.f)
	{
		return;
	}

	Health = FMath::Max(0.f, Health - Amount);
	RefreshLabel();

	if (Health <= 0.f)
	{
		bDestroyed = true;
		HealthText->SetText(FText::FromString(TEXT("CORE DOWN")));
		HealthText->SetTextRenderColor(FColor(220, 40, 40));
		for (TActorIterator<AAIPWaveDirector> It(GetWorld()); It; ++It)
		{
			It->NotifyTowerDestroyed();
			break;
		}
	}
}

void AAIPCoreTower::Repair(float Amount)
{
	if (bDestroyed || Amount <= 0.f)
	{
		return;
	}

	Health = FMath::Min(MaxHealth, Health + Amount);
	RefreshLabel();
}

void AAIPCoreTower::RefreshLabel()
{
	HealthText->SetText(FText::FromString(
		FString::Printf(TEXT("CORE  %.0f / %.0f"), Health, MaxHealth)));
}
