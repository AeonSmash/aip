#include "AIPWeapon.h"

#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "ProceduralMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/SoftObjectPath.h"

AAIPWeapon::AAIPWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	// After the camera and anim update, so the viewmodel does not lag the aim by a frame.
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;

	ViewMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ViewMesh"));
	SetRootComponent(ViewMesh);
	ViewMesh->SetMobility(EComponentMobility::Movable);
	ViewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ViewMesh->SetCastShadow(false);
	ViewMesh->SetOnlyOwnerSee(false);
	ViewMesh->SetRelativeScale3D(ViewScale);

	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProcMesh"));
	ProcMesh->SetupAttachment(ViewMesh);
	ProcMesh->SetMobility(EComponentMobility::Movable);
	ProcMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProcMesh->SetCastShadow(false);
	ProcMesh->SetOnlyOwnerSee(false);
	ProcMesh->SetCanEverAffectNavigation(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		ViewMesh->SetStaticMesh(CubeMesh.Object);
	}
}

void AAIPWeapon::BeginPlay()
{
	Super::BeginPlay();
	if (ViewMesh)
	{
		ViewMesh->SetMobility(EComponentMobility::Movable);
	}
	if (ProcMesh)
	{
		ProcMesh->SetMobility(EComponentMobility::Movable);
	}
	ApplyViewMesh();
	ViewMesh->SetRelativeScale3D(ViewScale);
	SetEquipped(bEquipped);
}

void AAIPWeapon::ApplyViewMesh()
{
	if (ViewMesh && !ViewMeshAsset.IsNull())
	{
		if (UStaticMesh* Mesh = Cast<UStaticMesh>(ViewMeshAsset.TryLoad()))
		{
			ViewMesh->SetStaticMesh(Mesh);
			const FBox Box = Mesh->GetBoundingBox();
			// Meshes are authored barrel-on-+X, so the tip is the +X face.
			MuzzleLocal = FVector(Box.Max.X, Box.GetCenter().Y, Box.GetCenter().Z);
			const float Size = Mesh->GetBoundingBox().GetSize().GetMax();
			if (Size > KINDA_SMALL_NUMBER)
			{
				ViewScale = FVector(32.f / Size);
			}
			ViewMesh->SetRelativeScale3D(ViewScale);
			if (ProcMesh)
			{
				ProcMesh->SetHiddenInGame(true);
			}
			UE_LOG(LogTemp, Log, TEXT("AIP weapon using static mesh %s (size %.1f)"), *Mesh->GetName(), Size);
			return;
		}
	}

	if (LoadObjViewMesh())
	{
		return;
	}

	// Cube placeholder — keep it small so a missed import is obvious, not a wall.
	ViewScale = FVector(0.22f, 0.1f, 0.08f);
	ViewRotation = FRotator::ZeroRotator;
}

bool AAIPWeapon::LoadObjViewMesh()
{
	if (!ProcMesh || ViewObjFile.IsEmpty())
	{
		return false;
	}

	TArray<FString> Candidates;
	Candidates.Add(FPaths::Combine(FPaths::ProjectDir(), TEXT("../art/weapons"), FPaths::GetCleanFilename(ViewObjFile)));
	Candidates.Add(FPaths::Combine(FPaths::ProjectContentDir(), ViewObjFile));

	FString Text;
	FString UsedPath;
	for (const FString& Path : Candidates)
	{
		if (FFileHelper::LoadFileToString(Text, *Path))
		{
			UsedPath = Path;
			break;
		}
	}
	if (UsedPath.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("AIP weapon OBJ missing: %s"), *ViewObjFile);
		return false;
	}

	TArray<FVector> Verts;
	TArray<int32> Tris;
	TArray<FString> Lines;
	Text.ParseIntoArrayLines(Lines, true);
	for (const FString& Line : Lines)
	{
		if (Line.StartsWith(TEXT("v ")))
		{
			TArray<FString> Parts;
			Line.ParseIntoArrayWS(Parts);
			if (Parts.Num() >= 4)
			{
				Verts.Add(FVector(FCString::Atod(*Parts[1]), FCString::Atod(*Parts[2]), FCString::Atod(*Parts[3])));
			}
		}
		else if (Line.StartsWith(TEXT("f ")))
		{
			TArray<FString> Parts;
			Line.ParseIntoArrayWS(Parts);
			if (Parts.Num() >= 4)
			{
				auto IndexOf = [](const FString& Token) -> int32
				{
					int32 Slash = INDEX_NONE;
					Token.FindChar(TCHAR('/'), Slash);
					const FString Num = Slash == INDEX_NONE ? Token : Token.Left(Slash);
					return FCString::Atoi(*Num) - 1;
				};
				const int32 A = IndexOf(Parts[1]);
				for (int32 i = 2; i + 1 < Parts.Num(); ++i)
				{
					Tris.Add(A);
					Tris.Add(IndexOf(Parts[i]));
					Tris.Add(IndexOf(Parts[i + 1]));
				}
			}
		}
	}

	if (Verts.Num() < 3 || Tris.Num() < 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIP weapon OBJ empty: %s"), *UsedPath);
		return false;
	}

	TArray<FVector> Normals;
	Normals.Init(FVector::ZeroVector, Verts.Num());
	for (int32 i = 0; i + 2 < Tris.Num(); i += 3)
	{
		const int32 IA = Tris[i];
		const int32 IB = Tris[i + 1];
		const int32 IC = Tris[i + 2];
		if (!Verts.IsValidIndex(IA) || !Verts.IsValidIndex(IB) || !Verts.IsValidIndex(IC))
		{
			continue;
		}
		const FVector N = FVector::CrossProduct(Verts[IB] - Verts[IA], Verts[IC] - Verts[IA]).GetSafeNormal();
		Normals[IA] += N;
		Normals[IB] += N;
		Normals[IC] += N;
	}
	for (FVector& N : Normals)
	{
		N = N.GetSafeNormal();
		if (N.IsNearlyZero())
		{
			N = FVector::UpVector;
		}
	}

	TArray<FVector2D> UV;
	TArray<FProcMeshTangent> Tangents;
	TArray<FLinearColor> Colors;
	UV.Init(FVector2D(0.f, 0.f), Verts.Num());
	Tangents.Init(FProcMeshTangent(1.f, 0.f, 0.f), Verts.Num());
	Colors.Init(FLinearColor(0.18f, 0.19f, 0.21f, 1.f), Verts.Num());

	// Average the vertices nearest the +X end so the muzzle lands on the barrel
	// centre line rather than a corner of the bounding box.
	float MaxX = -BIG_NUMBER;
	for (const FVector& V : Verts)
	{
		MaxX = FMath::Max(MaxX, V.X);
	}
	FVector TipSum = FVector::ZeroVector;
	int32 TipCount = 0;
	for (const FVector& V : Verts)
	{
		if (V.X > MaxX - 1.f)
		{
			TipSum += V;
			++TipCount;
		}
	}
	MuzzleLocal = TipCount > 0 ? TipSum / TipCount : FVector(MaxX, 0.f, 0.f);

	ProcMesh->CreateMeshSection_LinearColor(0, Verts, Tris, Normals, UV, Colors, Tangents, false);
	ProcMesh->SetHiddenInGame(false);

	if (UMaterialInterface* Mat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")))
	{
		if (UMaterialInstanceDynamic* Mid = ProcMesh->CreateDynamicMaterialInstance(0, Mat))
		{
			Mid->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.18f, 0.19f, 0.22f));
		}
	}

	ViewMesh->SetStaticMesh(nullptr);
	UE_LOG(LogTemp, Log, TEXT("AIP weapon loaded OBJ %s (%d verts)"), *UsedPath, Verts.Num());
	return true;
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

void AAIPWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bEquipped)
	{
		return;
	}
	if (GetAttachParentActor() == nullptr)
	{
		AttachToOwnerCamera();
	}
	RecoilAlpha = FMath::FInterpTo(RecoilAlpha, 0.f, DeltaTime, RecoilRecovery);
	UpdateViewTransform();
}

void AAIPWeapon::AddRecoil(float Scale)
{
	RecoilAlpha = FMath::Min(1.f, RecoilAlpha + FMath::Max(0.f, Scale));
}

FVector AAIPWeapon::GetMuzzleWorldLocation() const
{
	const USceneComponent* MeshComp = ViewMesh;
	if (ProcMesh && ProcMesh->GetNumSections() > 0 && !ProcMesh->bHiddenInGame)
	{
		MeshComp = ProcMesh;
	}
	if (MeshComp && !MuzzleLocal.IsNearlyZero())
	{
		return MeshComp->GetComponentTransform().TransformPosition(MuzzleLocal);
	}
	return GetActorLocation() + GetActorForwardVector() * 20.f;
}

void AAIPWeapon::UpdateViewTransform()
{
	const ACharacter* Pawn = GetPawnOwner();
	const APlayerController* PC = Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr;
	if (!PC)
	{
		return;
	}

	FVector ViewLoc;
	FRotator ViewRot;
	PC->GetPlayerViewPoint(ViewLoc, ViewRot);

	FVector Offset = ViewOffset;
	Offset.X -= RecoilKickBack * RecoilAlpha;
	const FRotator Kick(RecoilKickUp * RecoilAlpha, 0.f, 0.f);

	const FQuat Desired = FQuat(ViewRot) * FQuat(ViewRotation) * FQuat(Kick);
	SetActorLocationAndRotation(ViewLoc + ViewRot.RotateVector(Offset), Desired);
}

void AAIPWeapon::AttachToOwnerCamera(USceneComponent* Camera)
{
	USceneComponent* Anchor = Camera;
	if (!Anchor)
	{
		if (ACharacter* Pawn = GetPawnOwner())
		{
			Anchor = Pawn->FindComponentByClass<UCameraComponent>();
		}
	}
	if (!Anchor)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIP weapon %s: no camera to attach to"), *GetName());
		return;
	}

	if (ViewMesh)
	{
		ViewMesh->SetMobility(EComponentMobility::Movable);
		ViewMesh->SetCastShadow(false);
	}

	// Attach for lifetime and coarse movement only. The first person camera hangs off the
	// head bone with a 90/-90 fixup and uses control rotation just for rendering, so
	// inheriting its transform aims the barrel sideways. Track the view point instead.
	AttachToComponent(Anchor, FAttachmentTransformRules::KeepWorldTransform);
	SetActorEnableCollision(false);
	UpdateViewTransform();
	UE_LOG(LogTemp, Log, TEXT("AIP weapon %s tracking view of %s"), *GetName(), *Anchor->GetName());
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
