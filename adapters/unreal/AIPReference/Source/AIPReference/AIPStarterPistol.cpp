#include "AIPStarterPistol.h"

#include "AIPPistolSlugProjectile.h"
#include "AIPSfx.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "UObject/SoftObjectPath.h"

AAIPStarterPistol::AAIPStarterPistol()
{
	// Lightning-gun FBX (art/weapons/main-SNIPERriffle.fbx). Cyan sniper is unused.
	ViewMeshAsset = FSoftObjectPath(TEXT("/Game/AIP/Weapons/LightningGun.LightningGun"));
	ViewObjFile = TEXT("AIP/Weapons/LightningGun.obj");
	ViewScale = FVector(1.f);
	ViewOffset = FVector(22.f, 10.f, -8.f);
	// Grip origin, barrel on +X after the extract/import yaw. No corrective tilt.
	ViewRotation = FRotator::ZeroRotator;
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

	// Leave from the barrel tip but still converge on the crosshair.
	const FVector SpawnLoc = GetMuzzleWorldLocation();
	FVector ShotDir = (End - SpawnLoc).GetSafeNormal();
	if (ShotDir.IsNearlyZero())
	{
		ShotDir = Direction;
	}

	AAIPPistolSlugProjectile* Shot = World->SpawnActor<AAIPPistolSlugProjectile>(
		AAIPPistolSlugProjectile::StaticClass(),
		SpawnLoc,
		ShotDir.Rotation(),
		Params);

	if (!Shot)
	{
		return;
	}

	LastFireTime = Now;
	Shot->InitShot(ShotDir, Damage, GetOwner(), Range);
	AddRecoil();
	AIPSfx::Play(this, TEXT("rifle"), 0.5f);
}
