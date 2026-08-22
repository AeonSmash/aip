#include "AIPInvader.h"

#include "AIPCoreTower.h"
#include "AIPInvaderController.h"
#include "AIPWaveDirector.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"

AAIPInvader::AAIPInvader()
{
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = AAIPInvaderController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 88.f);

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(GetCapsuleComponent());
	BodyMesh->SetRelativeScale3D(FVector(0.7f, 0.7f, 1.6f));
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		BodyMesh->SetStaticMesh(CubeMesh.Object);
	}

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 380.f;
	GetCharacterMovement()->MaxAcceleration = 2048.f;
}

void AAIPInvader::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
	CachedTower = FindTower();
}

void AAIPInvader::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Health <= 0.f)
	{
		return;
	}

	AAIPCoreTower* Tower = CachedTower.Get();
	if (!Tower || Tower->IsDestroyedCore())
	{
		CachedTower = FindTower();
		Tower = CachedTower.Get();
		if (!Tower)
		{
			return;
		}
	}

	const FVector ToTower = Tower->GetActorLocation() - GetActorLocation();
	const float Dist = ToTower.Size();
	if (Dist > MeleeRange)
	{
		AddMovementInput(ToTower.GetSafeNormal(), 1.f);
		return;
	}

	MeleeTimer -= DeltaTime;
	if (MeleeTimer <= 0.f)
	{
		MeleeTimer = MeleeInterval;
		Tower->ReceiveInvaderDamage(MeleeDamage);
	}
}

void AAIPInvader::ReceiveWeaponDamage(float Amount, AActor* DamageCauser)
{
	if (Health <= 0.f || Amount <= 0.f)
	{
		return;
	}

	Health -= Amount;
	if (Health <= 0.f)
	{
		Die();
	}
}

void AAIPInvader::Die()
{
	Health = 0.f;
	if (UWorld* World = GetWorld())
	{
		for (TActorIterator<AAIPWaveDirector> It(World); It; ++It)
		{
			It->NotifyInvaderKilled();
			break;
		}
	}
	Destroy();
}

AAIPCoreTower* AAIPInvader::FindTower() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	AAIPCoreTower* Best = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();
	const FVector Loc = GetActorLocation();
	for (TActorIterator<AAIPCoreTower> It(World); It; ++It)
	{
		const float DistSq = FVector::DistSquared(Loc, It->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			Best = *It;
		}
	}
	return Best;
}
