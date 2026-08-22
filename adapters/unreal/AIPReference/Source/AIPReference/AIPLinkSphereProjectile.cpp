#include "AIPLinkSphereProjectile.h"

#include "AIPCoreTower.h"
#include "AIPInvader.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

AAIPLinkSphereProjectile::AAIPLinkSphereProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->InitSphereRadius(50.f);
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Collision->SetCollisionObjectType(ECC_WorldDynamic);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Collision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Collision->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	Collision->SetGenerateOverlapEvents(true);
	Collision->SetCanEverAffectNavigation(false);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCastShadow(false);
	Mesh->SetCanEverAffectNavigation(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		Mesh->SetStaticMesh(SphereMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ShapeMat(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (ShapeMat.Succeeded())
	{
		Mesh->SetMaterial(0, ShapeMat.Object);
	}

	Glow = CreateDefaultSubobject<UPointLightComponent>(TEXT("Glow"));
	Glow->SetupAttachment(Collision);
	Glow->SetIntensity(9000.f);
	Glow->SetAttenuationRadius(280.f);
	Glow->SetCastShadows(false);
	Glow->SetLightColor(FLinearColor(0.2f, 1.f, 0.35f));

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->UpdatedComponent = Collision;
	Movement->InitialSpeed = Speed;
	Movement->MaxSpeed = Speed;
	Movement->ProjectileGravityScale = 0.f;
	Movement->bRotationFollowsVelocity = true;
	Movement->bShouldBounce = false;

	SetActorEnableCollision(true);
	SetActorScale3D(FVector(StartScale));
}

void AAIPLinkSphereProjectile::InitShot(const FVector& Direction, bool bInRepair, float InDamage, float InRepair, AActor* InIgnore, float InRange)
{
	bRepair = bInRepair;
	Damage = InDamage;
	Repair = InRepair;
	IgnoredActor = InIgnore;
	MaxRange = FMath::Max(1.f, InRange);
	MuzzleOrigin = GetActorLocation();
	bConsumed = false;

	if (InIgnore)
	{
		Collision->IgnoreActorWhenMoving(InIgnore, true);
	}

	Collision->OnComponentBeginOverlap.AddDynamic(this, &AAIPLinkSphereProjectile::OnSphereOverlap);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	const FVector Dir = Direction.GetSafeNormal();
	Movement->InitialSpeed = Speed;
	Movement->MaxSpeed = Speed;
	Movement->Velocity = Dir * Speed;
	SetActorRotation(Dir.Rotation());
	SetLifeSpan((MaxRange / Speed) + 0.35f);

	const FLinearColor Color = bRepair
		? FLinearColor(0.35f, 1.f, 0.45f)
		: FLinearColor(0.12f, 0.95f, 0.28f);

	if (UMaterialInstanceDynamic* Mid = Mesh->CreateDynamicMaterialInstance(0))
	{
		Mid->SetVectorParameterValue(TEXT("Color"), Color);
	}
	Glow->SetLightColor(Color);
	RefreshScale();
}

void AAIPLinkSphereProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bConsumed)
	{
		return;
	}

	RefreshScale();
	if (FVector::Dist(GetActorLocation(), MuzzleOrigin) >= MaxRange)
	{
		Destroy();
	}
}

void AAIPLinkSphereProjectile::RefreshScale()
{
	const float Distance = FVector::Dist(GetActorLocation(), MuzzleOrigin);
	const float Alpha = FMath::Clamp(Distance / MaxRange, 0.f, 1.f);
	const float Scale = FMath::Lerp(StartScale, EndScale, Alpha);
	SetActorScale3D(FVector(Scale));
	Glow->SetIntensity(FMath::Lerp(9000.f, 1800.f, Alpha));
	Glow->SetAttenuationRadius(FMath::Lerp(280.f, 90.f, Alpha));
}

void AAIPLinkSphereProjectile::OnSphereOverlap(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Other || Other == this || Other == IgnoredActor.Get() || Other->GetOwner() == IgnoredActor.Get())
	{
		return;
	}
	if (Other->IsA<AAIPLinkSphereProjectile>())
	{
		return;
	}
	ApplyAndDestroy(Other);
}

void AAIPLinkSphereProjectile::ApplyAndDestroy(AActor* HitActor)
{
	if (bConsumed)
	{
		return;
	}
	bConsumed = true;

	if (HitActor)
	{
		if (bRepair)
		{
			if (AAIPCoreTower* Tower = Cast<AAIPCoreTower>(HitActor))
			{
				Tower->Repair(Repair);
			}
		}
		else if (AAIPInvader* Invader = Cast<AAIPInvader>(HitActor))
		{
			Invader->ReceiveWeaponDamage(Damage, IgnoredActor.Get());
		}
	}

	Destroy();
}
