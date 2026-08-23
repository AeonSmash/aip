# AIP Decentraland reference (Box 1)

SDK7 scene **0.2.4**. This world is a **source** (click Box 1 → `signal.box`) and a **destination** (Unreal `signal.terminal` → in-world breaker request). It does not ship LinkBeam.

## Loop

1. Click **Box 1** → emit `signal.box` (posted to the envelope board with write key `dcl-demo`).
2. Unreal maps that event to **revealing its AIP terminal**.
3. When Unreal activates the terminal and posts `signal.terminal`, this scene maps it locally: plaque text tells you to throw the switch at https://aeonsmash.com/#aip-switch — click the plaque to open it.
4. Pull the CRT breaker there. Unreal maps `signal.breaker` to **LinkBeam**.

The player never downloads a JSON file in this world.

## Preview

```bash
cd adapters/decentraland
npm install
npm start
```

Then Play the Unreal reference (pistol, no visible terminal until Box 1).

`src/config.ts` points at `https://arkavia-backend.vercel.app/aip/board` and session `demo`. Write keys stamp `source.world`. Apply Arkavia migration `020_aip_demo_board.sql` so the board survives cold starts.

Simulate Unreal **E** while the scene is running:

```bash
curl -X POST "https://arkavia-backend.vercel.app/aip/board/envelopes?session=demo" -H "Content-Type: application/json" -H "X-AIP-Write-Key: unreal-demo" -d "{\"envelope\":{\"aip\":\"0.1\",\"kind\":\"event\",\"id\":\"aip:unreal:terminal:demo\",\"type\":\"signal.terminal\",\"label\":\"Terminal used\"}}"
```

The plaque should switch to the breaker request. Click it to open the CRT page.

## Files

| Path | Role |
| --- | --- |
| `src/index.ts` | Box 1, plaque, poll |
| `src/sfx.ts` | Shared demo SFX (pistol, pulse, Rhodes, footsteps) |
| `src/envelope.ts` | `signal.box` builder |
| `src/board.ts` | POST/GET preview board (`X-AIP-Write-Key`) |
| `examples/box-1.aip.json` | Sample outbound envelope |
| `mappings/decentraland-aip.json` | How this destination interprets `signal.terminal` |
| `preview-board.mjs` | Local envelope board (DCL, Unreal, CRT) |

## Non-goals

No Unreal sockets. No LinkBeam in DCL. No player file download. No wallet/Supabase Auth (Tier 2).
