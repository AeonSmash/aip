# Changelog

All notable changes to the public AIP presentation are documented here.

The **envelope schema** remains **0.1**. This file versions the public repo
surface (docs, examples, SDK, adapters, explainer)—not the protocol wire format.

The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/)
and [Semantic Versioning](https://semver.org/) for the presentation/package
surface (`MAJOR.MINOR.PATCH`).

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
