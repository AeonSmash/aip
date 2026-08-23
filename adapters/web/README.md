# AIP web switch (source world)

A public CRT breaker. Pulling the lever is a **source-world event**. It does not name Unreal guns. A destination (the Unreal FPS reference) may interpret `signal.breaker` as unlocking a local LinkBeam.

## Public demo (aeonsmash.com)

https://aeonsmash.com/#aip-switch

Pull once. The page POSTs a new timestamped `signal.breaker` to
`https://arkavia-backend.vercel.app/aip/board` (`X-AIP-Write-Key: web-demo`).
Unreal polls that same HTTPS board and maps the event to LinkBeam. No player JSON download.

DCL plaque opens this URL after Unreal **E**.

## Local CRT

**http://127.0.0.1:8788/switch** (when `npm run board` is running) still posts to the local board, or to the hosted board if opened from another origin.

## Non-goals

No Unreal sockets. The webpage emits an envelope onto the board; Unreal maps it locally. File inbox is an operator fallback only.
