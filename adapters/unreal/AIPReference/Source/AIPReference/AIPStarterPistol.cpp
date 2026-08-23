#include "AIPStarterPistol.h"

#include "AIPPistolSlugProjectile.h"
#include "AIPSfx.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

AAIPStarterPistol::AAIPStarterPistol()
{
	ViewScale = FVector(0.22f, 0.1f, 0.08f);
	ViewOffset = FVector(36.f, 18.f, -14.f);
}

void AAIPStarterPistol::StartFire()
{
	if (!bEquipped)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const float Now = World->GetTimeSeconds();
	if (Now - LastFireTime < Cooldown)
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
	AAIPPistolSlugProjectile* Shot = World->SpawnActor<AAIPPistolSlugProjectile>(
		AAIPPistolSlugProjectile::StaticClass(),
		SpawnLoc,
		Direction.Rotation(),
		Params);

	if (!Shot)
	{
		return;
	}

	LastFireTime = Now;
	Shot->InitShot(Direction, Damage, GetOwner(), Range);
	AIPSfx::Play(this, TEXT("pistol"), 0.42f);
}
