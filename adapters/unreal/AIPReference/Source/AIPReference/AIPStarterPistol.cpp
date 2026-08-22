#include "AIPStarterPistol.h"

#include "AIPInvader.h"
#include "Engine/World.h"

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
	LastFireTime = Now;

	FVector Start;
	FVector End;
	if (!GetAim(Start, End, Range))
	{
		return;
	}

	FHitResult Hit;
	const bool bHit = LineTrace(Range, Hit);
	const FVector BeamEnd = bHit ? Hit.ImpactPoint : End;
	DrawBeam(Start, BeamEnd, FColor(220, 180, 80), 0.08f, 1.1f);

	if (bHit)
	{
		if (AAIPInvader* Invader = Cast<AAIPInvader>(Hit.GetActor()))
		{
			Invader->ReceiveWeaponDamage(Damage, GetOwner());
		}
	}
}
