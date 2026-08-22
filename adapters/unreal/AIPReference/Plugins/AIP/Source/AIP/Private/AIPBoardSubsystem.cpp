#include "AIPBoardSubsystem.h"

#include "AIPBlueprintLibrary.h"
#include "AIPPlayerUpgradeComponent.h"
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
	TEXT("http://127.0.0.1:8788"),
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
		if (Env.Type == TEXT("signal.box") && !bTerminalRevealed)
		{
			RevealTerminal();
		}
		if (Env.Type == TEXT("signal.breaker") && !bBreakerApplied)
		{
			ApplyBreaker(Env);
		}
	}
}

void UAIPBoardSubsystem::RevealTerminal()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	for (TActorIterator<AAIPTerminal> It(World); It; ++It)
	{
		It->SetRevealed(true);
		bTerminalRevealed = true;
		UE_LOG(LogTemp, Log, TEXT("AIP: revealed terminal from signal.box"));
		return;
	}
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
	if (!UAIPBlueprintLibrary::MapEnvelopeForUnrealFps(Envelope, Mapped, Error))
	{
		UE_LOG(LogTemp, Warning, TEXT("AIP breaker map failed: %s"), *Error);
		return;
	}

	if (Mapped.Upgrade != TEXT("unlock-linkbeam"))
	{
		UE_LOG(LogTemp, Warning, TEXT("AIP breaker mapped to %s (expected unlock-linkbeam)"), *Mapped.Upgrade);
		return;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	UAIPPlayerUpgradeComponent* Upgrade = Pawn ? Pawn->FindComponentByClass<UAIPPlayerUpgradeComponent>() : nullptr;
	if (!Upgrade)
	{
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
