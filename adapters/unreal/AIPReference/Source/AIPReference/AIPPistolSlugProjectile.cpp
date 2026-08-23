#include "AIPPistolSlugProjectile.h"

#include "AIPInvader.h"
#include "AIPLinkSphereProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

AAIPPistolSlugProjectile::AAIPPistolSlugProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->InitSphereRadius(8.f);
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
	// Engine cylinder is Z-up; projectile forward is X.
	Mesh->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	Mesh->SetRelativeScale3D(FVector(0.07f, 0.07f, 0.28f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CylinderMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ShapeMat(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (ShapeMat.Succeeded())
	{
		Mesh->SetMaterial(0, ShapeMat.Object);
	}

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->UpdatedComponent = Collision;
	Movement->InitialSpeed = Speed;
	Movement->MaxSpeed = Speed;
	Movement->ProjectileGravityScale = GravityScale;
	Movement->bRotationFollowsVelocity = true;
	Movement->bShouldBounce = false;

	SetActorEnableCollision(true);
	SetActorScale3D(FVector(StartScale));
}

void AAIPPistolSlugProjectile::InitShot(const FVector& Direction, float InDamage, AActor* InIgnore, float InRange)
{
	Damage = InDamage;
	IgnoredActor = InIgnore;
	MaxRange = FMath::Max(1.f, InRange);
	MuzzleOrigin = GetActorLocation();
	bConsumed = false;

	if (InIgnore)
	{
		Collision->IgnoreActorWhenMoving(InIgnore, true);
	}

	Collision->OnComponentBeginOverlap.AddDynamic(this, &AAIPPistolSlugProjectile::OnSphereOverlap);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	const FVector Dir = Direction.GetSafeNormal();
	Movement->InitialSpeed = Speed;
	Movement->MaxSpeed = Speed;
	Movement->ProjectileGravityScale = GravityScale;
	Movement->Velocity = Dir * Speed;
	SetActorRotation(Dir.Rotation());
	SetLifeSpan((MaxRange / Speed) + 0.25f);

	const FLinearColor Brass(0.82f, 0.55f, 0.18f);
	if (UMaterialInstanceDynamic* Mid = Mesh->CreateDynamicMaterialInstance(0))
	{
		Mid->SetVectorParameterValue(TEXT("Color"), Brass);
	}
	RefreshScale();
}

void AAIPPistolSlugProjectile::Tick(float DeltaTime)
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

void AAIPPistolSlugProjectile::RefreshScale()
{
	const float Distance = FVector::Dist(GetActorLocation(), MuzzleOrigin);
	const float Alpha = FMath::Clamp(Distance / MaxRange, 0.f, 1.f);
	const float Scale = FMath::Lerp(StartScale, EndScale, Alpha);
	SetActorScale3D(FVector(Scale));
}

void AAIPPistolSlugProjectile::OnSphereOverlap(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Other || Other == this || Other == IgnoredActor.Get() || Other->GetOwner() == IgnoredActor.Get())
	{
		return;
	}
	if (Other->IsA<AAIPPistolSlugProjectile>() || Other->IsA<AAIPLinkSphereProjectile>())
	{
		return;
	}
	ApplyAndDestroy(Other);
}

void AAIPPistolSlugProjectile::ApplyAndDestroy(AActor* HitActor)
{
	if (bConsumed)
	{
		return;
	}
	bConsumed = true;

	if (AAIPInvader* Invader = Cast<AAIPInvader>(HitActor))
	{
		Invader->ReceiveWeaponDamage(Damage, IgnoredActor.Get());
	}

	Destroy();
}
