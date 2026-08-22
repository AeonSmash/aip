# Recording the MVP loop (M7)

Target length: **60–120 seconds**. No narration required if on-screen text is clear.

## Prep

```bash
cd sdk/typescript
npm run build
npm run aip -- switch serve
```

Browser: https://aeonsmash.com/#aip-switch — pull the CRT breaker, then write inbox:

```bash
cd sdk/typescript
npm run aip -- exchange write-inbox ../../examples/main-breaker.aip.json
```

Open `adapters/unreal/AIPReference/AIPReference.uproject` → Play.

## Shot list

| Time | Picture | What to show |
| --- | --- | --- |
| 0:00 | HUD | Core HP, countdown, gun=Pistol |
| 0:05 | Pistol | Weak hitscan on an invader; no RMB repair |
| 0:15 | Web switch | ASCII lever clunks CLOSED; envelope types out |
| 0:30 | Terminal | Walk to cube, press E |
| 0:38 | HUD | Main Breaker → weapon.linkbeam / unlock-linkbeam, ignore equip |
| 0:45 | LinkBeam | Green energy spheres every 0.5s (shrink with range); RMB repair on the core |
| 1:05 | F | Outbox write; optional CLI map to Decentraland museum |
| 1:15 | End card | github.com/AeonSmash/aip |

Upload unlisted YouTube when ready; keep the existing explainer (`3D2cgt-ajlE`) as the grant media link until this cut replaces it.
