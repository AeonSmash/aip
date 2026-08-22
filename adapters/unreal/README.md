# AIP Unreal reference adapter

Unreal Engine **5.8** First Person C++ project plus a Runtime **AIP** plugin.

## What it demonstrates

1. Start with a weak **pistol** (no core repair)
2. NPC waves walk to the core and melee it
3. Pull the **web CRT breaker** (`adapters/web/`) so an envelope lands in `exchange/inbox/`
4. Press **E** near the AIP terminal → load inbox; `signal.breaker` maps to **LinkBeam unlock** (`equip` ignored)
5. **1** / **2** (or mouse wheel) switch pistol / LinkBeam after unlock; RMB repairs the core
6. Press **F** → write `achievement.sigil` to `exchange/outbox/`
7. Map that file with the TypeScript CLI for Decentraland (`collectible.museum`)

## Open / play

1. Open https://aeonsmash.com/#aip-switch — pull the CRT breaker (downloads the envelope)
2. `cd sdk/typescript && npm run aip -- exchange write-inbox ../../examples/main-breaker.aip.json`
   (or drop the downloaded file into `exchange/inbox/`)
3. Double-click [`AIPReference/AIPReference.uproject`](AIPReference/AIPReference.uproject)
4. Allow module rebuild if prompted (fully close and reopen the editor after config changes)
5. Play `Content/FirstPerson/Lvl_FirstPerson`
6. Core, terminal, and spawn points appear if the map does not already have them

Offline inbox without the page:

```bash
cd sdk/typescript
npm run aip -- exchange write-inbox ../../examples/main-breaker.aip.json
```

## Build from CLI

Requires Visual Studio 2022 Build Tools (MSVC), Windows SDK, and .NET Framework 4.8 Developer Pack.

```bat
"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" AIPReferenceEditor Win64 Development -Project="...\AIPReference.uproject" -WaitMutex
```

Verified compile: **Succeeded** (2026-08-22) on UE 5.8.1. PIE Play verified after CoreRedirects. Presentation **0.5.1**.

## Layout

| Path | Role |
| --- | --- |
| `AIPReference/` | Game project (First Person template, variants removed) |
| `AIPReference/Plugins/AIP/` | Envelope load, mapping, terminal, HUD, export |
| `../exchange/` | File transport (no DirectoryWatcher) |
| `../mappings/unreal-fps.json` | Destination-owned FPS rules (LinkBeam unlock) |
| `../web/` | CRT breaker source page |
| `../art/` | Blender FBX drop folder (import to `Content/AIP/`) |

## Controls

| Key | Action |
| --- | --- |
| LMB | Fire equipped gun |
| RMB | LinkBeam: repair core (when aimed at it; after unlock) |
| 1 / 2 or mouse wheel | Switch pistol / LinkBeam (linker after AIP unlock) |
| E | Load inbox envelope + apply mapping |
| F | Export arena sigil to outbox |

## Non-goals

No Unreal sockets, no live Hyperfy/DCL emission, no UT2004 assets, no Onslaught/vehicles/second core.
