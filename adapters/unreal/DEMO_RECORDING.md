# Recording the MVP loop (M7)

Target length: **60–120 seconds**. No narration required if on-screen text is clear.

## Prep

```bash
cd sdk/typescript
npm run aip -- exchange write-inbox ../../examples/emberblade.aip.json
```

Open `adapters/unreal/AIPReference/AIPReference.uproject` → Play.

## Shot list

| Time | Picture | What to show |
| --- | --- | --- |
| 0:00 | Title card or HUD | "AIP Unreal reference" |
| 0:05 | Walk to terminal | Greybox arena, UT-feel movement |
| 0:20 | Press E | Log/HUD: Emberblade → weapon.upgrade, ignore equip |
| 0:40 | Fire / move | Gun feels upgraded (damage multiplier applied in component) |
| 0:55 | Press F | Outbox write path on terminal text |
| 1:10 | Split or cut to terminal | `npm run aip -- map --destination decentraland exchange/outbox/...` → museum collectible |
| 1:25 | End card | github.com/AeonSmash/aip |

Upload unlisted YouTube when ready; keep the existing explainer (`3D2cgt-ajlE`) as the grant media link until this cut replaces it.
