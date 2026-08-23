#include "AIPLinkBeamWeapon.h"

#include "AIPLinkSphereProjectile.h"
#include "AIPSfx.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

AAIPLinkBeamWeapon::AAIPLinkBeamWeapon()
{
	ViewScale = FVector(0.42f, 0.1f, 0.1f);
	ViewOffset = FVector(40.f, 18.f, -14.f);
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

	const FVector SpawnLoc = Start + Direction * MuzzleOffset;
	AAIPLinkSphereProjectile* Shot = World->SpawnActor<AAIPLinkSphereProjectile>(
		AAIPLinkSphereProjectile::StaticClass(),
		SpawnLoc,
		Direction.Rotation(),
		Params);

	if (!Shot)
	{
		return;
	}

	PulseTimer = PulseInterval;
	Shot->InitShot(Direction, bRepair, DamagePerShot, RepairPerShot, GetOwner(), Range);
	if (!bRepair)
	{
		AIPSfx::Play(this, TEXT("linkbeam_pulse"), 0.48f);
	}
}
