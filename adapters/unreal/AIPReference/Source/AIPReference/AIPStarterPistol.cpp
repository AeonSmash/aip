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
	// Blender FBX is Y-forward; camera is X-forward.
	ViewRotation = FRotator(0.f, -90.f, 0.f);
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
