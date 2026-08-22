#include "AIPWeapon.h"

#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "UObject/ConstructorHelpers.h"

AAIPWeapon::AAIPWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	ViewMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ViewMesh"));
	SetRootComponent(ViewMesh);
	ViewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ViewMesh->SetCastShadow(false);
	ViewMesh->SetOnlyOwnerSee(true);
	ViewMesh->SetRelativeScale3D(ViewScale);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		ViewMesh->SetStaticMesh(CubeMesh.Object);
	}
}

void AAIPWeapon::BeginPlay()
{
	Super::BeginPlay();
	ViewMesh->SetRelativeScale3D(ViewScale);
	SetEquipped(bEquipped);
}

void AAIPWeapon::StartFire()
{
	bFiring = true;
}

void AAIPWeapon::StopFire()
{
	bFiring = false;
}

void AAIPWeapon::StartAltFire()
{
	bAltFiring = true;
}

void AAIPWeapon::StopAltFire()
{
	bAltFiring = false;
}

void AAIPWeapon::AttachToOwnerCamera()
{
	ACharacter* Pawn = GetPawnOwner();
	if (!Pawn)
	{
		return;
	}

	if (UCameraComponent* Cam = Pawn->FindComponentByClass<UCameraComponent>())
	{
		AttachToComponent(Cam, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		SetActorRelativeLocation(ViewOffset);
		SetActorRelativeRotation(FRotator::ZeroRotator);
	}
}

void AAIPWeapon::SetEquipped(bool bInEquipped)
{
	bEquipped = bInEquipped;
	SetActorHiddenInGame(!bEquipped);
	if (!bEquipped)
	{
		StopFire();
		StopAltFire();
	}
}

ACharacter* AAIPWeapon::GetPawnOwner() const
{
	return Cast<ACharacter>(GetOwner());
}

bool AAIPWeapon::GetAim(FVector& OutStart, FVector& OutEnd, float Range) const
{
	const ACharacter* Pawn = GetPawnOwner();
	if (!Pawn)
	{
		return false;
	}

	if (const APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
	{
		FRotator Rot;
		PC->GetPlayerViewPoint(OutStart, Rot);
		OutEnd = OutStart + Rot.Vector() * Range;
		return true;
	}

	OutStart = Pawn->GetActorLocation();
	OutEnd = OutStart + Pawn->GetActorForwardVector() * Range;
	return true;
}

bool AAIPWeapon::LineTrace(float Range, FHitResult& OutHit) const
{
	FVector Start;
	FVector End;
	if (!GetAim(Start, End, Range))
	{
		return false;
	}

	FCollisionQueryParams Params(SCENE_QUERY_STAT(AIPWeaponTrace), true, GetOwner());
	Params.AddIgnoredActor(this);
	if (const AActor* Pawn = GetOwner())
	{
		Params.AddIgnoredActor(Pawn);
	}

	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjParams.AddObjectTypesToQuery(ECC_PhysicsBody);

	return GetWorld() && GetWorld()->LineTraceSingleByObjectType(OutHit, Start, End, ObjParams, Params);
}

void AAIPWeapon::DrawBeam(const FVector& Start, const FVector& End, const FColor& Color, float Duration, float Thickness) const
{
	if (UWorld* World = GetWorld())
	{
		DrawDebugLine(World, Start, End, Color, false, Duration, 0, Thickness);
	}
}
