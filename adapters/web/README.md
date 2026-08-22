# AIP web switch (source world)

A public CRT breaker. Pulling the lever is a **source-world event**. It does not name Unreal guns. A destination (the Unreal FPS reference) may interpret `signal.breaker` as unlocking a local LinkBeam.

## Local demo (three-window loop)

With the envelope board running (`grants/decentraland`: `npm run board`):

**http://127.0.0.1:8788/switch**

Pull once. The page POSTs a **new** timestamped `signal.breaker` (`X-AIP-Write-Key: web-demo`). Unreal polls `GET /latest` and maps that event. No player JSON download.

`aip switch serve` still hosts this HTML on :8787; the page posts to :8788, not to inbox `/pull`.

## Public page (aeonsmash.com)

https://aeonsmash.com/#aip-switch

Same POST. A page served over HTTPS cannot reach `http://127.0.0.1:8788` (mixed content). Point it at a hosted board with `?board=` or `VITE_AIP_BOARD_URL`. Until that exists, use the local CRT above.

Static example copy (not “already pulled”): https://aeonsmash.com/aip/main-breaker.aip.json

## Non-goals

No Unreal sockets. The webpage emits an envelope onto the board; Unreal maps it locally. File inbox is an operator fallback only.
