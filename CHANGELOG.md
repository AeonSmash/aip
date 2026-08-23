# Changelog

All notable changes to the public AIP presentation are documented here.

The **envelope schema** remains **0.1**. This file versions the public repo
surface (docs, examples, SDK, adapters, explainer)—not the protocol wire format.

The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/)
and [Semantic Versioning](https://semver.org/) for the presentation/package
surface (`MAJOR.MINOR.PATCH`).

## [0.5.10] - 2026-08-23

First-person viewmodels use the Blender gun FBX files. Envelope schema remains **0.1**.

### Changed

- Starter pistol viewmodel is `main-SNIPERriffle.fbx` (lightning gun). Cyan
  sniper stays unused in the 60-second loop.
- LinkBeam viewmodel is `main-LINKgun.fbx`
- First-person guns load `LightningGun.obj` / `LinkGun.obj` extracted from the
  Blender FBX so PIE does not wait on an editor FBX import
- Pistol shot is a visible brass slug (no orange point light)

### Fixed

- Unreal reveals the AIP terminal from the hosted board's `signal.box`. That
  requires Arkavia backend **1.26.5** (Supabase persist) and migration **021**.

### Notes

- Protocol envelope version is still **0.1**.
- Close the editor, compile, or run `art/weapons/import_weapon_fbx.py` from
  the Output Log if the meshes are not yet in Content.

## [0.5.9] - 2026-08-23

Decentraland Box 1 scene is in the public repo. Envelope schema remains **0.1**.

### Added

- `adapters/decentraland/` — SDK7 Box 1 reference (scene **0.2.4**), including
  shared ElevenLabs SFX

### Changed

- `@aeonsmash/aip` and `AIP.uplugin` VersionName **0.5.8 → 0.5.9**

### Notes

- Protocol envelope version is still **0.1**.
- Preview: `cd adapters/decentraland && npm install && npm start`

## [0.5.8] - 2026-08-23

ElevenLabs sound-generation clips in the demo. Envelope schema remains **0.1**.

### Added

- `grants/.env` `ELEVENLABS_API_KEY` for `sound_generation` (not committed)

### Changed

- Demo SFX regenerated via ElevenLabs (`pistol`, `linkbeam_pulse`,
  `linkbeam_link`, `footstep`, `terminal_rhodes`)
- `@aeonsmash/aip` **0.5.5 → 0.5.8** (aligns SDK with Unreal 0.5.6 pistol
  slugs and 0.5.7 SFX player)
- `AIP.uplugin` VersionName **0.5.7 → 0.5.8**

### Notes

- Protocol envelope version is still **0.1**.
- Decentraland scene **0.2.4** plays the same clips as MP3.

## [0.5.7] - 2026-08-23

Shared ElevenLabs SFX for Unreal and Decentraland. Envelope schema remains **0.1**.

### Added

- Runtime WAV player (`UAIPSfxSubsystem`) loading `Content/AIP/Audio/*.wav`
- Pistol fire, LinkBeam pulse, LinkBeam RMB loop, footsteps, Rhodes terminal sting
- `adapters/audio/generate-sfx.mjs` — ElevenLabs if the key has
  `sound_generation`, otherwise a local analog/Rhodes synth

### Changed

- `AIP.uplugin` VersionName **0.5.6 → 0.5.7**

### Notes

- Protocol envelope version is still **0.1**.
- Decentraland scene **0.2.3** uses the same clip names as MP3.

## [0.5.6] - 2026-08-23

Starter pistol fires a visible slug instead of a debug line. Envelope schema remains **0.1**.

### Added

- `AAIPPistolSlugProjectile` — small brass cylinder, faster than LinkBeam orbs,
  with a slight gravity drop

### Changed

- Starter pistol LMB: discrete slug at 0.22s instead of a yellow debug trace
- `AIP.uplugin` VersionName **0.5.5 → 0.5.6**

### Notes

- Protocol envelope version is still **0.1**.
- LinkBeam spheres are unchanged (green, larger, no gravity, 0.5s volley).

## [0.5.5] - 2026-08-22

Public CRT on aeonsmash.com can complete the loop. Envelope schema remains **0.1**.

### Changed

- Unreal default `AIP.BoardUrl` is
  `https://arkavia-backend.vercel.app/aip/board` (same bus as DCL and the
  website). Local `:8788` remains optional via CVar.
- `@aeonsmash/aip` and `AIP.uplugin` VersionName **0.5.4 → 0.5.5**

### Notes

- Protocol envelope version is still **0.1**.
- Requires Arkavia **1.26.0** (`GET/POST /aip/board/*`) and Epoch-Website **1.9.5**.

## [0.5.4] - 2026-08-22

Plaque and terminal copy for the three-world loop. Envelope schema remains **0.1**.

### Changed

- After **E**, the Unreal terminal prompt is “Visit Decentraland scene for next clue”
- `@aeonsmash/aip` and `AIP.uplugin` VersionName **0.5.3 → 0.5.4**

### Notes

- Protocol envelope version is still **0.1**.
- DCL scene **0.2.1**: switch plaque hidden until `signal.terminal`; sign faces the player.

## [0.5.3] - 2026-08-22

Three-world HTTP envelope board. Envelope schema remains **0.1**.

### Added

- Unreal `UAIPBoardSubsystem` polls `GET /latest` (~1s). `signal.box` reveals
  the hidden AIP terminal. **E** POSTs `signal.terminal`. `signal.breaker`
  maps to LinkBeam unlock
- Mapping rule: `signal.box` → `upgrade: reveal-terminal` (`world.terminal`)
- Local CRT (`adapters/web/switch`, also `GET /switch` on the board) POSTs a
  new timestamped `signal.breaker` (`X-AIP-Write-Key: web-demo`)

### Changed

- Live loop no longer requires a player JSON download or inbox **E** load
- Unreal starts pistol-only; terminal stays hidden until `signal.box`
- `@aeonsmash/aip` and `AIP.uplugin` VersionName **0.5.2 → 0.5.3**

### Notes

- Protocol envelope version is still **0.1**.
- No Unreal game sockets. Demo-grade write keys, not accounts (Tier 2).
- HTTPS aeonsmash.com cannot POST to `http://127.0.0.1:8788` (mixed content).
  Local CRT or a hosted board URL (`?board=` / `VITE_AIP_BOARD_URL`).
- Epoch-Website **1.9.4** (CRT POST; not necessarily deployed). DCL plaque
  opens https://aeonsmash.com/#aip-switch

## [0.5.2] - 2026-08-22

LinkBeam fires discrete green energy spheres. Envelope schema remains **0.1**.

### Added

- `AAIPLinkSphereProjectile` — round green energy shot with muzzle glow; scale
  lerps down as it travels away from the player

### Changed

- LinkBeam LMB/RMB: 0.5s volley of projectiles instead of a debug line beam
  (hold fire reads as multiple shots in the air)
- `@aeonsmash/aip` and `AIP.uplugin` VersionName **0.5.1 → 0.5.2**

### Notes

- Protocol envelope version is still **0.1**.
- Unreal still loads files on **E**. No game sockets.
- Player file-download from the web switch is operator/demo transport, not a
  player loop (see Epoch-Website **1.9.3**).

## [0.5.1] - 2026-08-22

Public CRT breaker on aeonsmash.com. Envelope schema remains **0.1**.

### Added

- Live source page: https://aeonsmash.com/#aip-switch (Epoch-Website **1.9.2**,
  unlisted like `#aip`). Static envelope at `/aip/main-breaker.aip.json`
- `aip switch serve` CORS so the public page can optionally POST
  `http://127.0.0.1:8787/pull` and write `exchange/inbox/`

### Changed

- Demo loop no longer starts on localhost: pull the public lever, then
  `write-inbox` from `aip/sdk/typescript` (or drop the downloaded JSON into
  `exchange/inbox/`)
- `@aeonsmash/aip` and `AIP.uplugin` VersionName **0.5.0 → 0.5.1**

### Notes

- Protocol envelope version is still **0.1**.
- Unreal still loads files on **E**. No game sockets.

## [0.5.0] - 2026-08-22

Web CRT breaker as a second source world; Unreal starts with a pistol and **E**
unlocks LinkBeam. Envelope schema remains **0.1**.

### Added

- `adapters/web/switch/` — phosphor CRT page with a four-frame ASCII lever
  (click / Space / Enter). Pulling it emits `examples/main-breaker.aip.json`
- `aip switch serve [--port 8787]` — serves the page; `POST /pull` validates
  and writes `exchange/inbox/`. Static hosts fall back to JSON download
- `AAIPStarterPistol` — weak camera hitscan, no core repair
- Mapping rule: `signal.breaker` + high/exceptional → `weapon.linkbeam` /
  `unlock-linkbeam` (`equip` ignored)

### Changed

- Arena loadout: pistol at spawn; LinkBeam locked until the breaker mapping
- Emberblade on `unreal-fps` is notice-only so the live demo is one unlock
- `@aeonsmash/aip` and `AIP.uplugin` **0.4.0 → 0.5.0** (plugin Version 4 → 5)
- Docs and recording shot list: web lever → E → LinkBeam repair → F

### Notes

- Protocol envelope version is still **0.1**. Do not treat presentation `0.5.x`
  as a schema bump.
- Unreal still loads files on **E**. No game sockets. CyanSniper C++ remains in
  tree but is not spawned in this loop.

## [0.4.0] - 2026-08-22

Playable Unreal one-tower reference in PIE. Envelope schema remains **0.1**.

Presentation, `@aeonsmash/aip`, and the Unreal `AIP` plugin are aligned at
**0.4.0** (SDK and plugin had stayed on 0.2.0 through 0.3.0).

### Changed

- `@aeonsmash/aip` **0.2.0 → 0.4.0**; `AIP.uplugin` VersionName **0.2.0 → 0.4.0**
  (integer Version 2 → 4)
- Root README status: one-tower Unreal loop is playable in the editor, not only
  compiled
- Toolchain pass criteria: Play after a full editor restart (CoreRedirects load
  at startup)

### Fixed

- First Person Blueprints still parented to `/Script/TP_FirstPerson` after the
  C++ rename to `AIPReference`. Without redirects, `BP_FirstPersonCharacter`,
  `BP_FirstPersonPlayerController`, and `BP_FirstPersonGameMode` failed to load
  (`CreateExport: Failed to load Outer`). `DefaultEngine.ini` `[CoreRedirects]`
  maps those classes to `AIPReferenceCharacter` / PlayerController / GameMode /
  CameraManager. Verified in PIE on 2026-08-22.

### Notes

- Protocol envelope version is still **0.1**. Do not treat presentation `0.4.x`
  as a schema bump.
- After a clean load, **File → Save All** rewrites the Blueprint parents so
  they no longer depend on the redirects.

## [0.3.0] - 2026-08-22

One-tower AIP arena on the Unreal reference. Envelope schema remains **0.1**.

### Added

- `AAIPWeapon` base: camera-origin traces, cube viewmodel attach, debug beam
- **LinkBeam** — LMB pulses damage invaders; RMB repairs the home core
- **CyanSniper** — hitscan; locked until an AIP mapping with `unlock-sniper`
- `AAIPCoreTower` (HP label, invader damage, LinkBeam repair)
- `AAIPInvader` (walk-to-core, melee), `AAIPInvaderStart` spawn markers
- `AAIPWaveDirector` — 3 waves, rest after wave 1 so the terminal beat is
  playable; HUD line (core HP, countdown/wave, equipped gun)
- `AAIPTowerDefendGameMode` — default pawn/controller from First Person BPs;
  `AAIPReferenceGameMode` spawns tower / starts / director / terminal if the
  map has none (level-placed markers win)
- Art drop folder `adapters/unreal/art/` for later original Blender FBX

### Changed

- `mappings/unreal-fps.json` high/exceptional `artifact.weapon` →
  `weapon.sniper` / `unlock-sniper` (no longer a 1.75× damage multiplier);
  `equip` still ignored
- TypeScript test `maps Emberblade for unreal-fps sniper unlock` matches the
  new mapping
- HUD: arena line plus AIP sovereignty summary (origin / local type / ignored
  capabilities)
- Character: LMB/RMB/1/2/mouse wheel; template pistol meshes hidden
- Docs: adapter README controls, toolchain compile verified 2026-08-22
- `DEMO_RECORDING.md` shot list rewritten for the arena loop (LinkBeam →
  terminal **E** → CyanSniper → **F** export)
- `adapters/unreal/art/README.md` — FBX import notes (map vs viewmodels)

### Notes

- Non-goals remain: no networking, no live Hyperfy/DCL emission, no UT2004
  assets, no Onslaught/vehicles/second core.

## [0.2.1] - 2026-08-21

### Changed

- Changelog rewritten for the Unreal MVP surface: complete Added / Changed /
  Fixed / Notes sections so a stranger can see what shipped without reading the
  full tree.
- `@aeonsmash/aip` package version aligned to **0.2.0** (was still `0.1.0` while
  the presentation already advertised the 0.2 SDK surface).

## [0.2.0] - 2026-08-21

First **working reference loop**: validate → map → Unreal inbound → Unreal
outbound → map for a second destination. Envelope schema is unchanged at 0.1.

### Added

#### TypeScript AIP Core (`sdk/typescript/`)

- `@aeonsmash/aip` package: `validateEnvelope`, `mapEnvelope`, shared types,
  and a CLI (`aip`).
- Schema validation against `schemas/aip-envelope-0.1.schema.json` via Ajv 2020
  + formats; failures return readable path/message lists (never silent).
- Destination mapping resolver with optional `rules[]` match on `type`,
  `powerTier`, and `sourceWorld`; falls back to top-level `interpretation`
  (compatible with the original 0.1 example mapping files).
- CLI commands:
  - `aip validate <envelope.json>`
  - `aip map --destination <unreal|unreal-fps|decentraland> <envelope.json>`
  - `aip exchange write-inbox <envelope.json>`
  - `aip exchange list`
- Automated tests (`node --test`): Emberblade accepts; mutated envelope
  rejects; Unreal emblem map; Unreal-FPS upgrade map; Decentraland museum map
  for an Unreal-origin sigil.

#### File exchange (`exchange/`)

- `inbox/` and `outbox/` directories with documented filename rules
  (`exchange/README.md`).
- Windows-safe basename sanitization (`:` → `_`); example
  `aip:epochs:emberblade:01` → `aip_epochs_emberblade_01.aip.json`.
- Transport is **button/poll**, not `IDirectoryWatcher` (editor-only module;
  breaks packaged builds).

#### Destination mapping data

- `mappings/unreal-fps.json` — destination-owned rules (not a C++ switch):
  high/exceptional `artifact.weapon` → local `weapon.upgrade` / `tier1` /
  `damageMultiplier` 1.75; lower tiers → notice only; `equip` ignored.
- Existing `examples/unreal-local-mapping.json` and
  `examples/decentraland-local-mapping.json` left unchanged for the public 0.1
  story.

#### Examples

- `examples/arena-sigil.aip.json` — Unreal-origin `achievement.sigil` for the
  reverse path (maps to Decentraland `collectible.museum`).

#### Unreal Engine 5.8 reference (`adapters/unreal/`)

- `AIPReference/` First Person C++ project (from `TP_FirstPerson`; Shooter and
  Horror variants removed to keep the MVP buildable).
- Runtime plugin `Plugins/AIP/` (`AIP.uplugin`):
  - `UAIPBlueprintLibrary` — resolve repo `exchange/` / `mappings/`, load
    envelope JSON, apply `unreal-fps` rules, export sigil to outbox
  - `AAIPTerminal` — mid-field interact volume; **E** load inbox, **F** export
  - `UAIPPlayerUpgradeComponent` — holds upgrade multiplier + last mapping
  - `UAIPSovereigntyWidget` — on-screen origin / local type / accepted vs
    ignored capabilities
- Game module wiring: UT-feel pacing (faster walk, FOV ~100), GameMode spawns
  terminal if none placed, character binds **E** / **F**.
- Docs: `adapters/unreal/README.md`, `TOOLCHAIN.md`, `DEMO_RECORDING.md`.
- Verified: `AIPReferenceEditor` Win64 Development **Succeeded** on UE 5.8.1
  (VS 2022 Build Tools + .NET Framework 4.8 Developer Pack).

#### Grant packaging (sibling folder)

- `../epic/SUBMIT_CHECKLIST.md` and `SUBMIT_STATUS.md` for Epic MegaGrants
  Cycle 2 (human submit still required).

### Changed

- Root `README.md` — inventory table includes SDK, exchange, Unreal adapter,
  FPS mapping, arena-sigil example; quick CLI loop; status notes the Unreal
  MVP in tree.
- `.gitignore` — Node (`node_modules/`, `sdk/typescript/dist/`), Unreal
  (`Binaries/`, `Intermediate/`, `DerivedDataCache/`, `Saved/`, etc.), and
  runtime exchange drops (`exchange/inbox/*.aip.json`,
  `exchange/outbox/*.aip.json`) while keeping `.gitkeep`.

### Fixed

- Envelope IDs with `:` cannot be used as Windows filenames; CLI and Unreal
  sanitizers strip unsafe characters before write.
- Unreal module API macro after template rename:
  `AIPReference_API` → `AIPREFERENCE_API` (UBT-defined) so the editor target
  links.
- `.uproject` written as UTF-8 without a Windows-1252 em dash (UBT JSON parse
  failure).

### Notes

- Non-goals remain: no networking, no live Hyperfy/DCL emission, no profile
  linking, no GLB-as-claim, no UT2004/ONS-Severance assets.
- Protocol envelope version is still **0.1**; do not treat presentation `0.2.x`
  as a schema bump.

## [0.1.1] - 2026-08-20

### Changed

- Explainer video starts the LCD soundtrack; pausing the video pauses the audio.
- Magnifier removed; the video takes more of the stage.
- Right-hand scenes stay in layout so the full heading and copy remain visible.
- Repo link sits on the same row as the caption, next to the audio deck.

## [0.1.0] - 2026-08-19

### Added

- Public draft landing, MIT license, and architecture notes.
- Envelope schema 0.1 with Emberblade and destination-mapping examples.
- Recordable explainer page and published walkthrough video.
