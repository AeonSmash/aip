# AIP Unreal reference adapter

Unreal Engine **5.8** First Person C++ project plus a Runtime **AIP** plugin.

## What it demonstrates

1. Start with a weak **pistol** (small brass slugs, no core repair). The AIP terminal is hidden.
2. NPC waves walk to the core and melee it.
3. Decentraland Box 1 posts `signal.box` → this destination **reveals the terminal**.
4. Press **E** at the terminal → POST `signal.terminal` to the envelope board (does not unlock LinkBeam).
5. Pull the **web CRT breaker** → POST `signal.breaker` → this destination maps that to **LinkBeam unlock** (`equip` ignored).
6. **1** / **2** (or mouse wheel) switch pistol / LinkBeam after unlock; RMB repairs the core.
7. Press **F** → write `achievement.sigil` to `exchange/outbox/`.

Inbox file drop remains as an operator fallback. The live three-window loop does not require a player JSON download.

## Operator loop

1. Play Unreal (`Lvl_FirstPerson`) — polls `https://arkavia-backend.vercel.app/aip/board`
2. DCL preview: click **Box 1** → terminal appears in Unreal
3. Walk up, **E** → DCL plaque appears; click it → https://aeonsmash.com/#aip-switch
4. Pull the lever on the **public** CRT → Unreal unlocks LinkBeam (keys 1/2)

CVars (defaults): `AIP.BoardUrl=https://arkavia-backend.vercel.app/aip/board`, `AIP.BoardSession=demo`, `AIP.BoardWriteKey=unreal-demo`.

## Open / play

1. Double-click [`AIPReference/AIPReference.uproject`](AIPReference/AIPReference.uproject)
2. Allow module rebuild if prompted (fully close and reopen the editor after config changes)
3. Play `Content/FirstPerson/Lvl_FirstPerson`
4. Core, hidden terminal, and spawn points appear if the map does not already have them

Offline inbox without the board (fallback):

```bash
cd sdk/typescript
npm run aip -- exchange write-inbox ../../examples/main-breaker.aip.json
```

Then call `TryLoadInboxAndApply` from a debug path; **E** in the live demo posts `signal.terminal` instead.

## Build from CLI

Requires Visual Studio 2022 Build Tools (MSVC), Windows SDK, and .NET Framework 4.8 Developer Pack.

```bat
"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" AIPReferenceEditor Win64 Development -Project="...\AIPReference.uproject" -WaitMutex
```

Verified compile: **Succeeded** (2026-08-22) on UE 5.8.1. Presentation **0.5.8**.

## Layout

| Path | Role |
| --- | --- |
| `AIPReference/` | Game project (First Person template, variants removed) |
| `AIPReference/Plugins/AIP/` | Envelope load, mapping, terminal, HUD, export, board poll |
| `../exchange/` | File transport (operator fallback; no DirectoryWatcher) |
| `../mappings/unreal-fps.json` | Destination-owned FPS rules (`signal.box` → reveal terminal; breaker → LinkBeam) |
| `../web/` | CRT breaker source page |
| `../art/` | Blender FBX drop folder (import to `Content/AIP/`) |

## Controls

| Key | Action |
| --- | --- |
| LMB | Fire equipped gun |
| RMB | LinkBeam: repair core (when aimed at it; after unlock) |
| 1 / 2 or mouse wheel | Switch pistol / LinkBeam (linker after AIP unlock) |
| E | POST `signal.terminal` (after the terminal is revealed) |
| F | Export arena sigil to outbox |

## Non-goals

No Unreal sockets, no live Hyperfy/DCL emission, no UT2004 assets, no Onslaught/vehicles/second core. No character/wallet login.
