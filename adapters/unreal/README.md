# AIP Unreal reference adapter

Unreal Engine **5.8** First Person C++ project plus a Runtime **AIP** plugin.

## What it demonstrates

1. Press **E** near the mid-field terminal → load `exchange/inbox/*.aip.json`
2. Destination-owned mapping from `mappings/unreal-fps.json` upgrades the local gun (foreign `equip` is ignored)
3. HUD shows origin world, local type, accepted vs ignored capabilities
4. Press **F** → write `achievement.sigil` to `exchange/outbox/`
5. Map that file with the TypeScript CLI for Decentraland (`collectible.museum`)

## Open / play

1. Double-click [`AIPReference/AIPReference.uproject`](AIPReference/AIPReference.uproject)
2. Allow module rebuild if prompted
3. Play `Content/FirstPerson/Lvl_FirstPerson`
4. Walk to the grey cube terminal (spawned at ~(0, 800, 100) if none is placed)

Prep inbox first:

```bash
cd sdk/typescript
npm run aip -- exchange write-inbox ../../examples/emberblade.aip.json
```

## Build from CLI

Requires Visual Studio 2022 Build Tools (MSVC), Windows SDK, and .NET Framework 4.8 Developer Pack.

```bat
"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" AIPReferenceEditor Win64 Development -Project="...\AIPReference.uproject" -WaitMutex
```

Verified compile: **Succeeded** (2026-08-20) on UE 5.8.1.

## Layout

| Path | Role |
| --- | --- |
| `AIPReference/` | Game project (First Person template, variants removed) |
| `AIPReference/Plugins/AIP/` | Envelope load, mapping, terminal, HUD, export |
| `../exchange/` | File transport (no DirectoryWatcher) |
| `../mappings/unreal-fps.json` | Destination-owned FPS rules |

## Controls

| Key | Action |
| --- | --- |
| E | Load inbox envelope + apply mapping |
| F | Export arena sigil to outbox |

## Non-goals

No networking, no live Hyperfy/DCL emission, no UT2004 assets, no Onslaught.
