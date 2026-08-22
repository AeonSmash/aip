#include "AIPLinkBeamWeapon.h"

#include "AIPCoreTower.h"
#include "AIPInvader.h"

AAIPLinkBeamWeapon::AAIPLinkBeamWeapon()
{
	ViewScale = FVector(0.42f, 0.1f, 0.1f);
	ViewOffset = FVector(40.f, 18.f, -14.f);
	bUnlocked = false;
}

void AAIPLinkBeamWeapon::SetUnlocked(bool bInUnlocked)
{
	bUnlocked = bInUnlocked;
}

void AAIPLinkBeamWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bEquipped || !bUnlocked)
	{
		return;
	}

	PulseTimer -= DeltaTime;
	if (PulseTimer > 0.f)
	{
		return;
	}

	if (bFiring)
	{
		PulseTimer = PulseInterval;
		Pulse(false);
	}
	else if (bAltFiring)
	{
		PulseTimer = PulseInterval;
		Pulse(true);
	}
}

void AAIPLinkBeamWeapon::Pulse(bool bRepair)
{
	FVector Start;
	FVector End;
	if (!GetAim(Start, End, Range))
	{
		return;
	}

	FHitResult Hit;
	const bool bHit = LineTrace(Range, Hit);
	const FVector BeamEnd = bHit ? Hit.ImpactPoint : End;
	const FColor Color = bRepair ? FColor(80, 220, 90) : FColor(40, 200, 70);
	DrawBeam(Start + GetActorForwardVector() * 20.f, BeamEnd, Color, PulseInterval + 0.02f, 2.2f);

	if (!bHit)
	{
		return;
	}

	AActor* HitActor = Hit.GetActor();
	if (bRepair)
	{
		if (AAIPCoreTower* Tower = Cast<AAIPCoreTower>(HitActor))
		{
			Tower->Repair(RepairPerPulse);
		}
		return;
	}

	if (AAIPInvader* Invader = Cast<AAIPInvader>(HitActor))
	{
		Invader->ReceiveWeaponDamage(DamagePerPulse, GetOwner());
	}
}
