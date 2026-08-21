#include "AIPBlueprintLibrary.h"

#include "Dom/JsonObject.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

namespace AIPPaths
{
	static FString FindRepoRoot()
	{
		// ProjectDir = .../AIPReference/
		// Expected: .../aip/adapters/unreal/AIPReference/
		FString Cursor = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
		for (int32 i = 0; i < 8; ++i)
		{
			const FString Exchange = FPaths::Combine(Cursor, TEXT("exchange"));
			const FString Schema = FPaths::Combine(Cursor, TEXT("schemas"), TEXT("aip-envelope-0.1.schema.json"));
			if (FPaths::DirectoryExists(Exchange) && FPaths::FileExists(Schema))
			{
				return Cursor;
			}
			Cursor = FPaths::GetPath(Cursor);
			if (Cursor.IsEmpty() || Cursor.EndsWith(TEXT(":")))
			{
				break;
			}
		}
		return FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	}
}

FString UAIPBlueprintLibrary::GetAIPExchangeRoot()
{
	return FPaths::Combine(AIPPaths::FindRepoRoot(), TEXT("exchange"));
}

FString UAIPBlueprintLibrary::GetAIPInboxDirectory()
{
	return FPaths::Combine(GetAIPExchangeRoot(), TEXT("inbox"));
}

FString UAIPBlueprintLibrary::GetAIPOutboxDirectory()
{
	return FPaths::Combine(GetAIPExchangeRoot(), TEXT("outbox"));
}

FString UAIPBlueprintLibrary::GetAIPMappingsDirectory()
{
	return FPaths::Combine(AIPPaths::FindRepoRoot(), TEXT("mappings"));
}

FString UAIPBlueprintLibrary::SanitizeEnvelopeIdForFilename(const FString& EnvelopeId)
{
	FString Out;
	Out.Reserve(EnvelopeId.Len());
	for (TCHAR Ch : EnvelopeId)
	{
		const bool bOk =
			(Ch >= 'A' && Ch <= 'Z') ||
			(Ch >= 'a' && Ch <= 'z') ||
			(Ch >= '0' && Ch <= '9') ||
			Ch == '.' || Ch == '_' || Ch == '-';
		Out.AppendChar(bOk ? Ch : TEXT('_'));
	}
	return Out;
}

static TSharedPtr<FJsonObject> ReadJsonFile(const FString& AbsolutePath, FString& OutError)
{
	FString Raw;
	if (!FFileHelper::LoadFileToString(Raw, *AbsolutePath))
	{
		OutError = FString::Printf(TEXT("Could not read file: %s"), *AbsolutePath);
		return nullptr;
	}

	TSharedPtr<FJsonObject> Root;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Raw);
	if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
	{
		OutError = FString::Printf(TEXT("Invalid JSON: %s"), *AbsolutePath);
		return nullptr;
	}
	return Root;
}

static void ReadStringArray(const TSharedPtr<FJsonObject>& Obj, const FString& Field, TArray<FString>& Out)
{
	Out.Reset();
	const TArray<TSharedPtr<FJsonValue>>* Arr = nullptr;
	if (Obj.IsValid() && Obj->TryGetArrayField(Field, Arr) && Arr)
	{
		for (const TSharedPtr<FJsonValue>& V : *Arr)
		{
			Out.Add(V->AsString());
		}
	}
}

static bool ParseEnvelopeObject(const TSharedPtr<FJsonObject>& Root, FAIPEnvelope& OutEnvelope, FString& OutError)
{
	if (!Root.IsValid())
	{
		OutError = TEXT("Null JSON object");
		return false;
	}

	OutEnvelope = FAIPEnvelope();
	OutEnvelope.Aip = Root->GetStringField(TEXT("aip"));
	OutEnvelope.Kind = Root->GetStringField(TEXT("kind"));
	OutEnvelope.Id = Root->GetStringField(TEXT("id"));
	OutEnvelope.Type = Root->GetStringField(TEXT("type"));
	OutEnvelope.Label = Root->GetStringField(TEXT("label"));

	if (OutEnvelope.Aip != TEXT("0.1") || OutEnvelope.Id.IsEmpty() || OutEnvelope.Type.IsEmpty() || OutEnvelope.Label.IsEmpty())
	{
		OutError = TEXT("Envelope missing required fields or wrong aip version (need 0.1).");
		return false;
	}

	const TSharedPtr<FJsonObject>* SourceObj = nullptr;
	if (!Root->TryGetObjectField(TEXT("source"), SourceObj) || !SourceObj || !SourceObj->IsValid())
	{
		OutError = TEXT("Envelope missing source object.");
		return false;
	}
	OutEnvelope.Source.World = (*SourceObj)->GetStringField(TEXT("world"));
	OutEnvelope.Source.App = (*SourceObj)->GetStringField(TEXT("app"));
	OutEnvelope.Source.Scene = (*SourceObj)->GetStringField(TEXT("scene"));
	OutEnvelope.Source.IssuedAt = (*SourceObj)->GetStringField(TEXT("issuedAt"));

	if (const TSharedPtr<FJsonObject>* Prov = nullptr; Root->TryGetObjectField(TEXT("provenance"), Prov) && Prov && Prov->IsValid())
	{
		OutEnvelope.Provenance.Origin = (*Prov)->GetStringField(TEXT("origin"));
		OutEnvelope.Provenance.DiscoveredBy = (*Prov)->GetStringField(TEXT("discoveredBy"));
		ReadStringArray(*Prov, TEXT("history"), OutEnvelope.Provenance.History);
	}

	if (const TSharedPtr<FJsonObject>* Ctx = nullptr; Root->TryGetObjectField(TEXT("context"), Ctx) && Ctx && Ctx->IsValid())
	{
		if (const TSharedPtr<FJsonObject>* Power = nullptr; (*Ctx)->TryGetObjectField(TEXT("power"), Power) && Power && Power->IsValid())
		{
			OutEnvelope.Context.Power.Scale = (*Power)->GetStringField(TEXT("scale"));
			OutEnvelope.Context.Power.Tier = (*Power)->GetStringField(TEXT("tier"));
			OutEnvelope.Context.Power.Basis = (*Power)->GetStringField(TEXT("basis"));
		}
	}

	ReadStringArray(Root, TEXT("capabilities"), OutEnvelope.Capabilities);

	if (const TSharedPtr<FJsonObject>* Rights = nullptr; Root->TryGetObjectField(TEXT("rights"), Rights) && Rights && Rights->IsValid())
	{
		OutEnvelope.Rights.Owner = (*Rights)->GetStringField(TEXT("owner"));
		(*Rights)->TryGetBoolField(TEXT("transferable"), OutEnvelope.Rights.Transferable);
		(*Rights)->TryGetBoolField(TEXT("displayable"), OutEnvelope.Rights.Displayable);
	}

	if (const TSharedPtr<FJsonObject>* Rep = nullptr; Root->TryGetObjectField(TEXT("representation"), Rep) && Rep && Rep->IsValid())
	{
		OutEnvelope.Representation.Glb = (*Rep)->GetStringField(TEXT("glb"));
	}

	return true;
}

bool UAIPBlueprintLibrary::LoadAipEnvelopeFromFile(const FString& AbsolutePath, FAIPEnvelope& OutEnvelope, FString& OutError)
{
	OutError.Reset();
	const TSharedPtr<FJsonObject> Root = ReadJsonFile(AbsolutePath, OutError);
	if (!Root.IsValid())
	{
		return false;
	}
	return ParseEnvelopeObject(Root, OutEnvelope, OutError);
}

bool UAIPBlueprintLibrary::LoadAipEnvelopeFromInbox(FAIPEnvelope& OutEnvelope, FString& OutError, const FString& PreferredBaseName)
{
	OutError.Reset();
	const FString Inbox = GetAIPInboxDirectory();
	IFileManager& FM = IFileManager::Get();

	if (!PreferredBaseName.IsEmpty())
	{
		const FString Candidate = FPaths::Combine(Inbox, PreferredBaseName.EndsWith(TEXT(".aip.json")) ? PreferredBaseName : PreferredBaseName + TEXT(".aip.json"));
		if (FPaths::FileExists(Candidate))
		{
			return LoadAipEnvelopeFromFile(Candidate, OutEnvelope, OutError);
		}
	}

	TArray<FString> Files;
	FM.FindFiles(Files, *FPaths::Combine(Inbox, TEXT("*.aip.json")), true, false);
	Files.Sort();
	if (Files.Num() == 0)
	{
		OutError = FString::Printf(TEXT("No *.aip.json files in inbox: %s"), *Inbox);
		return false;
	}

	const FString Path = FPaths::Combine(Inbox, Files[0]);
	return LoadAipEnvelopeFromFile(Path, OutEnvelope, OutError);
}

static bool RuleMatches(const FAIPEnvelope& Envelope, const TSharedPtr<FJsonObject>& Match)
{
	if (!Match.IsValid())
	{
		return false;
	}

	FString Type;
	if (Match->TryGetStringField(TEXT("type"), Type) && Type != Envelope.Type)
	{
		return false;
	}

	FString SourceWorld;
	if (Match->TryGetStringField(TEXT("sourceWorld"), SourceWorld) && SourceWorld != Envelope.Source.World)
	{
		return false;
	}

	const TArray<TSharedPtr<FJsonValue>>* Tiers = nullptr;
	if (Match->TryGetArrayField(TEXT("powerTier"), Tiers) && Tiers && Tiers->Num() > 0)
	{
		bool bHit = false;
		for (const TSharedPtr<FJsonValue>& V : *Tiers)
		{
			if (V->AsString() == Envelope.Context.Power.Tier)
			{
				bHit = true;
				break;
			}
		}
		if (!bHit)
		{
			return false;
		}
	}

	return true;
}

static void FillMappedFromInterpretation(
	const FAIPEnvelope& Envelope,
	const FString& Destination,
	const TSharedPtr<FJsonObject>& Interp,
	FAIPMappedInterpretation& OutMapped)
{
	OutMapped = FAIPMappedInterpretation();
	OutMapped.Destination = Destination;
	OutMapped.EnvelopeId = Envelope.Id;
	OutMapped.Principle = Interp->GetStringField(TEXT("principle"));
	if (OutMapped.Principle.IsEmpty())
	{
		OutMapped.Principle = TEXT("destination-sovereignty");
	}
	OutMapped.LocalType = Interp->GetStringField(TEXT("localType"));
	OutMapped.LocalActor = Interp->GetStringField(TEXT("localActor"));
	OutMapped.Slot = Interp->GetStringField(TEXT("slot"));
	OutMapped.Upgrade = Interp->GetStringField(TEXT("upgrade"));
	OutMapped.Note = Interp->GetStringField(TEXT("note"));
	Interp->TryGetNumberField(TEXT("damageMultiplier"), OutMapped.DamageMultiplier);
	if (OutMapped.DamageMultiplier <= 0.0f)
	{
		OutMapped.DamageMultiplier = 1.0f;
	}

	TArray<FString> Accepted;
	TArray<FString> Ignored;
	ReadStringArray(Interp, TEXT("acceptedCapabilities"), Accepted);
	ReadStringArray(Interp, TEXT("ignoredCapabilities"), Ignored);

	for (const FString& Cap : Envelope.Capabilities)
	{
		if (Accepted.Contains(Cap))
		{
			OutMapped.AcceptedCapabilities.Add(Cap);
		}
		else
		{
			OutMapped.IgnoredCapabilities.Add(Cap);
		}
	}

	OutMapped.SourceWorld = Envelope.Source.World;
	OutMapped.SourceLabel = Envelope.Label;
	OutMapped.SourceType = Envelope.Type;
	OutMapped.PowerTier = Envelope.Context.Power.Tier;
}

bool UAIPBlueprintLibrary::MapEnvelopeForUnrealFps(const FAIPEnvelope& Envelope, FAIPMappedInterpretation& OutMapped, FString& OutError)
{
	OutError.Reset();
	const FString MappingPath = FPaths::Combine(GetAIPMappingsDirectory(), TEXT("unreal-fps.json"));
	const TSharedPtr<FJsonObject> Root = ReadJsonFile(MappingPath, OutError);
	if (!Root.IsValid())
	{
		return false;
	}

	const FString Destination = Root->GetStringField(TEXT("destination"));
	const TArray<TSharedPtr<FJsonValue>>* Rules = nullptr;
	if (Root->TryGetArrayField(TEXT("rules"), Rules) && Rules)
	{
		for (const TSharedPtr<FJsonValue>& RuleVal : *Rules)
		{
			const TSharedPtr<FJsonObject> Rule = RuleVal->AsObject();
			if (!Rule.IsValid())
			{
				continue;
			}
			const TSharedPtr<FJsonObject>* Match = nullptr;
			const TSharedPtr<FJsonObject>* Interp = nullptr;
			if (!Rule->TryGetObjectField(TEXT("match"), Match) || !Rule->TryGetObjectField(TEXT("interpretation"), Interp))
			{
				continue;
			}
			if (RuleMatches(Envelope, *Match))
			{
				FillMappedFromInterpretation(Envelope, Destination, *Interp, OutMapped);
				return !OutMapped.LocalType.IsEmpty();
			}
		}
	}

	const TSharedPtr<FJsonObject>* Fallback = nullptr;
	if (Root->TryGetObjectField(TEXT("interpretation"), Fallback) && Fallback && Fallback->IsValid())
	{
		FillMappedFromInterpretation(Envelope, Destination, *Fallback, OutMapped);
		return !OutMapped.LocalType.IsEmpty();
	}

	OutError = TEXT("No mapping rule matched this envelope for unreal-fps.");
	return false;
}

bool UAIPBlueprintLibrary::ExportSigilToOutbox(const FString& Label, FString& OutWrittenPath, FString& OutError)
{
	OutError.Reset();
	OutWrittenPath.Reset();

	const FString Id = TEXT("aip:unreal-reference:sigil:01");
	const FString Outbox = GetAIPOutboxDirectory();
	IFileManager::Get().MakeDirectory(*Outbox, true);

	TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
	Root->SetStringField(TEXT("aip"), TEXT("0.1"));
	Root->SetStringField(TEXT("kind"), TEXT("event"));
	Root->SetStringField(TEXT("id"), Id);
	Root->SetStringField(TEXT("type"), TEXT("achievement.sigil"));
	Root->SetStringField(TEXT("label"), Label.IsEmpty() ? TEXT("Arena Sigil") : Label);

	TSharedRef<FJsonObject> Source = MakeShared<FJsonObject>();
	Source->SetStringField(TEXT("world"), TEXT("unreal"));
	Source->SetStringField(TEXT("app"), TEXT("aip-unreal-reference"));
	Source->SetStringField(TEXT("scene"), TEXT("lvl-first-person"));
	Source->SetStringField(TEXT("issuedAt"), FDateTime::UtcNow().ToIso8601());
	Root->SetObjectField(TEXT("source"), Source);

	TSharedRef<FJsonObject> Provenance = MakeShared<FJsonObject>();
	Provenance->SetStringField(TEXT("origin"), TEXT("AIP Unreal Reference"));
	Provenance->SetStringField(TEXT("discoveredBy"), TEXT("player:local"));
	TArray<TSharedPtr<FJsonValue>> History;
	History.Add(MakeShared<FJsonValueString>(TEXT("earned in Unreal FPS arena")));
	Provenance->SetArrayField(TEXT("history"), History);
	Root->SetObjectField(TEXT("provenance"), Provenance);

	TArray<TSharedPtr<FJsonValue>> Caps;
	Caps.Add(MakeShared<FJsonValueString>(TEXT("display")));
	Caps.Add(MakeShared<FJsonValueString>(TEXT("quest-flag")));
	Caps.Add(MakeShared<FJsonValueString>(TEXT("equip")));
	Root->SetArrayField(TEXT("capabilities"), Caps);

	FString Json;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Json);
	if (!FJsonSerializer::Serialize(Root, Writer))
	{
		OutError = TEXT("Failed to serialize sigil envelope.");
		return false;
	}

	OutWrittenPath = FPaths::Combine(Outbox, SanitizeEnvelopeIdForFilename(Id) + TEXT(".aip.json"));
	if (!FFileHelper::SaveStringToFile(Json, *OutWrittenPath))
	{
		OutError = FString::Printf(TEXT("Failed to write outbox file: %s"), *OutWrittenPath);
		return false;
	}
	return true;
}
