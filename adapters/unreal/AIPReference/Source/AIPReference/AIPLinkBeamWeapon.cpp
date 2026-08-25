#include "AIPLinkBeamWeapon.h"

#include "AIPLinkSphereProjectile.h"
#include "AIPSfx.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "UObject/SoftObjectPath.h"

AAIPLinkBeamWeapon::AAIPLinkBeamWeapon()
{
	ViewMeshAsset = FSoftObjectPath(TEXT("/Game/AIP/Weapons/LinkGun.LinkGun"));
	ViewObjFile = TEXT("AIP/Weapons/LinkGun.obj");
	ViewScale = FVector(1.f);
	ViewOffset = FVector(24.f, 10.f, -8.f);
	ViewRotation = FRotator::ZeroRotator;
	bUnlocked = false;

	LinkHum = CreateDefaultSubobject<UAudioComponent>(TEXT("LinkHum"));
	LinkHum->SetupAttachment(ViewMesh);
	LinkHum->bAutoActivate = false;
	LinkHum->bIsUISound = true;
}

void AAIPLinkBeamWeapon::SetUnlocked(bool bInUnlocked)
{
	bUnlocked = bInUnlocked;
}

void AAIPLinkBeamWeapon::StartFire()
{
	Super::StartFire();
	TryLaunch(false);
}

void AAIPLinkBeamWeapon::StartAltFire()
{
	Super::StartAltFire();
	StartLinkHum();
	TryLaunch(true);
}

void AAIPLinkBeamWeapon::StopAltFire()
{
	Super::StopAltFire();
	StopLinkHum();
}

void AAIPLinkBeamWeapon::StartLinkHum()
{
	if (!bEquipped || !bUnlocked)
	{
		return;
	}
	AIPSfx::PlayLoop(this, LinkHum, TEXT("linkbeam_link"), 0.28f);
}

void AAIPLinkBeamWeapon::StopLinkHum()
{
	if (LinkHum)
	{
		LinkHum->Stop();
	}
}

void AAIPLinkBeamWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bEquipped || !bUnlocked)
	{
		StopLinkHum();
		return;
	}

	if (bAltFiring && LinkHum && !LinkHum->IsPlaying())
	{
		StartLinkHum();
	}

	PulseTimer -= DeltaTime;
	if (PulseTimer > 0.f)
	{
		return;
	}

	if (bFiring)
	{
		TryLaunch(false);
	}
	else if (bAltFiring)
	{
		TryLaunch(true);
	}
}

void AAIPLinkBeamWeapon::TryLaunch(bool bRepair)
{
	if (!bEquipped || !bUnlocked || PulseTimer > 0.f)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FVector Start;
	FVector End;
	if (!GetAim(Start, End, Range))
	{
		return;
	}

	const FVector Direction = (End - Start).GetSafeNormal();
	if (Direction.IsNearlyZero())
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.Owner = GetOwner();
	Params.Instigator = Cast<APawn>(GetOwner());
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Leave from the barrel tip but still converge on the crosshair.
	const FVector SpawnLoc = GetMuzzleWorldLocation();
	FVector ShotDir = (End - SpawnLoc).GetSafeNormal();
	if (ShotDir.IsNearlyZero())
	{
		ShotDir = Direction;
	}

	AAIPLinkSphereProjectile* Shot = World->SpawnActor<AAIPLinkSphereProjectile>(
		AAIPLinkSphereProjectile::StaticClass(),
		SpawnLoc,
		ShotDir.Rotation(),
		Params);

	if (!Shot)
	{
		return;
	}

	PulseTimer = PulseInterval;
	Shot->InitShot(ShotDir, bRepair, DamagePerShot, RepairPerShot, GetOwner(), Range);
	AddRecoil(0.6f);
	if (!bRepair)
	{
		AIPSfx::Play(this, TEXT("linkbeam_pulse"), 0.48f);
	}
}
