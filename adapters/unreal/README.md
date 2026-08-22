# AIP Unreal reference adapter

Unreal Engine **5.8** First Person C++ project plus a Runtime **AIP** plugin.

## What it demonstrates

1. Start with **LinkBeam** (LMB damages invaders, RMB repairs the home core)
2. NPC waves walk to the core and melee it
3. Press **E** near the AIP terminal → load `exchange/inbox/*.aip.json`
4. High-tier foreign weapon maps to **CyanSniper unlock** (`mappings/unreal-fps.json`; `equip` ignored)
5. **1** / **2** (or mouse wheel) switch guns after unlock
6. Press **F** → write `achievement.sigil` to `exchange/outbox/`
7. Map that file with the TypeScript CLI for Decentraland (`collectible.museum`)

## Open / play

1. Double-click [`AIPReference/AIPReference.uproject`](AIPReference/AIPReference.uproject)
2. Allow module rebuild if prompted (fully close and reopen the editor after config changes)
3. Play `Content/FirstPerson/Lvl_FirstPerson`
4. Core, terminal, and spawn points appear if the map does not already have them

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

Verified compile: **Succeeded** (2026-08-22) on UE 5.8.1. PIE Play verified after CoreRedirects (presentation **0.4.0**).

## Layout

| Path | Role |
| --- | --- |
| `AIPReference/` | Game project (First Person template, variants removed) |
| `AIPReference/Plugins/AIP/` | Envelope load, mapping, terminal, HUD, export |
| `../exchange/` | File transport (no DirectoryWatcher) |
| `../mappings/unreal-fps.json` | Destination-owned FPS rules (sniper unlock) |
| `../art/` | Blender FBX drop folder (import to `Content/AIP/`) |

## Controls

| Key | Action |
| --- | --- |
| LMB | Fire equipped gun |
| RMB | LinkBeam: repair core (when aimed at it) |
| 1 / 2 or mouse wheel | Switch LinkBeam / CyanSniper (sniper after AIP unlock) |
| E | Load inbox envelope + apply mapping |
| F | Export arena sigil to outbox |

## Non-goals

No networking, no live Hyperfy/DCL emission, no UT2004 assets, no Onslaught/vehicles/second core.
