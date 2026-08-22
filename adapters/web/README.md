# AIP web switch (source world)

A public CRT breaker. Pulling the lever is a **source-world event**. It does not name Unreal guns. A destination (the Unreal FPS reference) may interpret `signal.breaker` as unlocking a local LinkBeam.

## Public page (aeonsmash.com)

https://aeonsmash.com/#aip-switch

Click / Space / Enter to pull. The envelope downloads. Drop it in Unreal inbox:

```bash
cd sdk/typescript
npm run aip -- exchange write-inbox ../../examples/main-breaker.aip.json
```

Or save the downloaded `aip_web_main-breaker_01.aip.json` into `exchange/inbox/`.

Static copy: https://aeonsmash.com/aip/main-breaker.aip.json

## Optional local inbox write

If `aip switch serve` is running on this machine, the public page will POST to `http://127.0.0.1:8787/pull` and write inbox without a download.

```bash
cd sdk/typescript
npm run build
npm run aip -- switch serve
```

Then Play Unreal, rest after wave 1, **E** at the terminal.

## Non-goals

No Unreal sockets. No live sync. The webpage emits an envelope; Unreal still loads files on **E**.
