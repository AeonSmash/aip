#include "AIPCyanSniperWeapon.h"

#include "AIPInvader.h"
#include "Engine/World.h"

AAIPCyanSniperWeapon::AAIPCyanSniperWeapon()
{
	ViewScale = FVector(0.7f, 0.08f, 0.08f);
	ViewOffset = FVector(48.f, 16.f, -12.f);
	bUnlocked = false;
}

void AAIPCyanSniperWeapon::SetUnlocked(bool bInUnlocked)
{
	bUnlocked = bInUnlocked;
}

void AAIPCyanSniperWeapon::StartFire()
{
	if (!bEquipped || !bUnlocked)
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
	DrawBeam(Start, BeamEnd, FColor(40, 220, 255), 0.18f, 1.6f);

	if (bHit)
	{
		if (AAIPInvader* Invader = Cast<AAIPInvader>(Hit.GetActor()))
		{
			Invader->ReceiveWeaponDamage(Damage, GetOwner());
		}
	}
}
