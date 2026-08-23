# Changelog — AIP Decentraland reference

Scene-only versioning. Envelope schema remains **0.1**.

## [0.2.4] — 2026-08-23

### Changed

- SFX clips are ElevenLabs MP3s (`sounds/*.mp3`), same names as Unreal WAVs

## [0.2.3] — 2026-08-23

### Added

- Themed SFX shared with Unreal: Box 1 uses the pistol clip, plaque appear
  uses the Rhodes terminal sting, plaque click uses the LinkBeam pulse,
  walking uses footsteps

## [0.2.2] — 2026-08-22

### Changed

- Envelope board URL is the hosted HTTPS bus:
  `https://arkavia-backend.vercel.app/aip/board`

## [0.2.1] — 2026-08-22

### Changed

- Switch plaque stays hidden until Unreal posts `signal.terminal`
- Sign yaw flipped so the throw-the-switch copy faces the player (readable)

## [0.2.0] — 2026-08-22

### Added

- Envelope board client: POST `signal.box` with `X-AIP-Write-Key: dcl-demo`,
  poll `GET /latest` for Unreal `signal.terminal`
- Plaque maps a terminal event to a breaker request and opens
  https://aeonsmash.com/#aip-switch (“Throw the switch”)

### Changed

- Local board (`preview-board.mjs`) stamps `source` from write keys and keeps
  one envelope type per session

## [0.1.0] — 2026-08-22

### Added

- SDK7 Box 1 scene (source `signal.box`, destination plaque)
