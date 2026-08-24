#include "AIPBoardSubsystem.h"

#include "AIPBlueprintLibrary.h"
#include "AIPPlayerUpgradeComponent.h"
#include "AIPSfx.h"
#include "AIPTerminal.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Http.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

TAutoConsoleVariable<FString> CVarAIPBoardUrl(
	TEXT("AIP.BoardUrl"),
	TEXT("https://arkavia-backend.vercel.app/aip/board"),
	TEXT("AIP envelope board base URL"),
	ECVF_Default);

TAutoConsoleVariable<FString> CVarAIPBoardSession(
	TEXT("AIP.BoardSession"),
	TEXT("demo"),
	TEXT("AIP envelope board session id"),
	ECVF_Default);

TAutoConsoleVariable<FString> CVarAIPBoardWriteKey(
	TEXT("AIP.BoardWriteKey"),
	TEXT("unreal-demo"),
	TEXT("X-AIP-Write-Key for posting signal.terminal"),
	ECVF_Default);

bool UAIPBoardSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const UWorld* World = Cast<UWorld>(Outer);
	return World && (World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE);
}

void UAIPBoardSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	PollTimer = 0.f;
	PlayStartedUtc = FDateTime::UtcNow();
}

TStatId UAIPBoardSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UAIPBoardSubsystem, STATGROUP_Tickables);
}

void UAIPBoardSubsystem::Tick(float DeltaTime)
{
	PollTimer -= DeltaTime;
	if (PollTimer > 0.f)
	{
		return;
	}
	PollTimer = 1.f;
	PollLatest();
}

void UAIPBoardSubsystem::PollLatest()
{
	if (bPollInFlight)
	{
		return;
	}

	const FString Url = FString::Printf(
		TEXT("%s/latest?session=%s"),
		*CVarAIPBoardUrl.GetValueOnGameThread().TrimStartAndEnd(),
		*CVarAIPBoardSession.GetValueOnGameThread().TrimStartAndEnd());

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Req = FHttpModule::Get().CreateRequest();
	Req->SetURL(Url);
	Req->SetVerb(TEXT("GET"));
	TWeakObjectPtr<UAIPBoardSubsystem> WeakThis(this);
	Req->OnProcessRequestComplete().BindLambda(
		[WeakThis](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded)
		{
			if (UAIPBoardSubsystem* Self = WeakThis.Get())
			{
				const int32 Code = Response.IsValid() ? Response->GetResponseCode() : 0;
				const FString Json = Response.IsValid() ? Response->GetContentAsString() : FString();
				Self->HandleLatestPayload(Json, Code, bSucceeded);
			}
		});
	bPollInFlight = true;
	Req->ProcessRequest();
}

void UAIPBoardSubsystem::HandleLatestPayload(const FString& Json, int32 ResponseCode, bool bSucceeded)
{
	bPollInFlight = false;
	if (!bSucceeded || ResponseCode != 200)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIP board poll failed code=%d ok=%s"), ResponseCode, bSucceeded ? TEXT("true") : TEXT("false"));
		return;
	}

	TArray<FAIPEnvelope> Envelopes;
	FString Error;
	if (!UAIPBlueprintLibrary::ParseBoardLatestPayload(Json, Envelopes, Error))
	{
		UE_LOG(LogTemp, Warning, TEXT("AIP board parse: %s"), *Error);
		return;
	}
	HandleEnvelopes(Envelopes);
}

void UAIPBoardSubsystem::HandleEnvelopes(const TArray<FAIPEnvelope>& Envelopes)
{
	for (const FAIPEnvelope& Env : Envelopes)
	{
		if (Env.Id.IsEmpty())
		{
			continue;
		}

		const bool bLive = IsLiveForThisPlay(Env);
		if (!bLive)
		{
			if (!SeenEnvelopeIds.Contains(Env.Id))
			{
				UE_LOG(LogTemp, Log, TEXT("AIP: ignoring leftover %s id=%s issuedAt=%s"), *Env.Type, *Env.Id, *Env.Source.IssuedAt);
			}
			SeenEnvelopeIds.Add(Env.Id);
			continue;
		}

		if (Env.Type == TEXT("signal.box") && !bTerminalRevealed)
		{
			RevealTerminal();
		}
		if (Env.Type == TEXT("signal.breaker") && !bBreakerApplied)
		{
			ApplyBreaker(Env);
			if (!bBreakerApplied)
			{
				continue;
			}
		}
		SeenEnvelopeIds.Add(Env.Id);
	}
}

bool UAIPBoardSubsystem::IsLiveForThisPlay(const FAIPEnvelope& Envelope) const
{
	FDateTime Issued;
	FString Raw = Envelope.Source.IssuedAt.TrimStartAndEnd();
	if (Raw.IsEmpty())
	{
		return false;
	}

	if (!FDateTime::ParseIso8601(*Raw, Issued))
	{
		int32 DotIndex = INDEX_NONE;
		if (Raw.FindChar(TEXT('.'), DotIndex))
		{
			FString Head = Raw.Left(DotIndex);
			if (Raw.EndsWith(TEXT("Z")))
			{
				Head += TEXT("Z");
			}
			if (!FDateTime::ParseIso8601(*Head, Issued))
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	const FDateTime Cutoff = PlayStartedUtc - FTimespan::FromSeconds(2.0);
	return Issued >= Cutoff;
}

void UAIPBoardSubsystem::RevealTerminal()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	AAIPTerminal* Terminal = nullptr;
	for (TActorIterator<AAIPTerminal> It(World); It; ++It)
	{
		Terminal = *It;
		break;
	}
	if (!Terminal)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Terminal = World->SpawnActor<AAIPTerminal>(AAIPTerminal::StaticClass(), FVector(0.f, 800.f, 100.f), FRotator::ZeroRotator, Params);
		UE_LOG(LogTemp, Log, TEXT("AIP: spawned terminal for signal.box reveal"));
	}
	if (!Terminal)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIP: signal.box received but terminal spawn failed"));
		return;
	}

	Terminal->SetRevealed(true);
	bTerminalRevealed = true;
	AIPSfx::Play(World, TEXT("linkbeam_pulse"), 0.4f);
	UE_LOG(LogTemp, Log, TEXT("AIP: revealed terminal from signal.box"));
}

void UAIPBoardSubsystem::ApplyBreaker(const FAIPEnvelope& Envelope)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FAIPMappedInterpretation Mapped;
	FString Error;
	const bool bMapped = UAIPBlueprintLibrary::MapEnvelopeForUnrealFps(Envelope, Mapped, Error);
	if (!bMapped || Mapped.Upgrade != TEXT("unlock-linkbeam"))
	{
		if (Envelope.Type != TEXT("signal.breaker"))
		{
			UE_LOG(LogTemp, Warning, TEXT("AIP breaker map failed: %s"), bMapped ? *Mapped.Upgrade : *Error);
			return;
		}
		Mapped.Upgrade = TEXT("unlock-linkbeam");
		Mapped.LocalType = TEXT("weapon.linkbeam");
		Mapped.LocalActor = TEXT("AIP_LinkBeam");
		Mapped.SourceType = Envelope.Type;
		Mapped.SourceLabel = Envelope.Label;
		Mapped.SourceWorld = Envelope.Source.World;
		UE_LOG(LogTemp, Log, TEXT("AIP: signal.breaker fallback unlock (map=%s)"), bMapped ? *Mapped.Upgrade : *Error);
	}

	APlayerController* PC = World->GetFirstPlayerController();
	APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	UAIPPlayerUpgradeComponent* Upgrade = Pawn ? Pawn->FindComponentByClass<UAIPPlayerUpgradeComponent>() : nullptr;
	if (!Upgrade)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIP: signal.breaker waiting for pawn upgrade component"));
		return;
	}

	bBreakerApplied = true;
	Upgrade->ApplyMapping(Envelope, Mapped);
	UE_LOG(LogTemp, Log, TEXT("AIP: applied signal.breaker from board"));
}

void UAIPBoardSubsystem::NotifyTerminalActivated()
{
	if (bTerminalPosted)
	{
		return;
	}
	bTerminalPosted = true;
	PostTerminalEnvelope();
}

void UAIPBoardSubsystem::PostTerminalEnvelope()
{
	const FString IssuedAt = FDateTime::UtcNow().ToIso8601();
	const FString Body = FString::Printf(
		TEXT("{\"envelope\":{\"aip\":\"0.1\",\"kind\":\"event\",\"id\":\"aip:unreal:terminal:%s\",\"type\":\"signal.terminal\",\"label\":\"Terminal used\",\"source\":{\"world\":\"unreal\",\"app\":\"aip-unreal-reference\",\"scene\":\"lvl-first-person\",\"issuedAt\":\"%s\"},\"capabilities\":[\"display\",\"quest-flag\"]}}"),
		*FDateTime::UtcNow().ToString(TEXT("%Y%m%d%H%M%S")),
		*IssuedAt);

	const FString Url = FString::Printf(
		TEXT("%s/envelopes?session=%s"),
		*CVarAIPBoardUrl.GetValueOnGameThread().TrimStartAndEnd(),
		*CVarAIPBoardSession.GetValueOnGameThread().TrimStartAndEnd());

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Req = FHttpModule::Get().CreateRequest();
	Req->SetURL(Url);
	Req->SetVerb(TEXT("POST"));
	Req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Req->SetHeader(TEXT("X-AIP-Write-Key"), CVarAIPBoardWriteKey.GetValueOnGameThread());
	Req->SetContentAsString(Body);
	TWeakObjectPtr<UAIPBoardSubsystem> WeakThis(this);
	Req->OnProcessRequestComplete().BindLambda(
		[WeakThis](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded)
		{
			if (UAIPBoardSubsystem* Self = WeakThis.Get())
			{
				const int32 Code = Response.IsValid() ? Response->GetResponseCode() : 0;
				Self->HandlePostResult(Code, bSucceeded);
			}
		});
	Req->ProcessRequest();
}

void UAIPBoardSubsystem::HandlePostResult(int32 ResponseCode, bool bSucceeded)
{
	UE_LOG(LogTemp, Log, TEXT("AIP board POST signal.terminal code=%d ok=%s"), ResponseCode, bSucceeded ? TEXT("true") : TEXT("false"));
}
