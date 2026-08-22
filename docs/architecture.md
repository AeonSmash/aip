# Architecture

AIP is a semantic envelope plus destination-owned mapping. It is not a shared world database and not an asset teleporter.

## Destination sovereignty

A source may describe:

- what the subject is (`type`)
- where it came from (`source`, `provenance`)
- how strong it was *in that world* (`context.power`, always relative)
- what it could do there (`capabilities`)
- who may display or transfer it (`rights`)

A destination may:

- accept the envelope
- map it to a local type
- ignore capabilities it does not implement
- refuse the envelope entirely

Source-world statistics are never authoritative gameplay values in another game.

Worked example only: an independent RPG describes Emberblade as `artifact.weapon` with relative power `high`. One destination might map that to a cosmetic emblem. Another might map a foreign sigil to a museum collectible. A web CRT switch can emit `signal.breaker`; Unreal FPS maps that event to unlocking a local LinkBeam and ignores `equip`. Neither destination reproduces the source's combat system. Neither destination is required by AIP Core.

## Engine-neutral core, runtime adapters

```text
                    ┌─────────────────────┐
                    │      AIP Core       │
                    │  envelope · schema  │
                    │  validation · docs  │
                    └──────────┬──────────┘
           ┌───────────────────┼───────────────────┐
           ▼                   ▼                   ▼
     Example adapter      Example adapter      Other adapters
     (optional)           (optional)           (optional)
```

Named worlds in this repo (Unreal, Decentraland, EPOCHS, web/Three.js) are demonstration adapters. None of them is a requirement of AIP Core. A real-time 3D adapter is useful because it makes the protocol visible. Visibility is not the same as a core dependency.

Optional mesh references (`representation.glb`) can travel with an envelope. Appearance is supporting evidence, not the claim. The claim is that **meaning** can cross worlds that do not share coordinates, engines, databases, or rules.

## What 0.1 is not

- Not a finished standard
- Not a live network protocol
- Not a blockchain layer
- Not tied to Decentraland, Unreal, or any other specific platform
- Not a requirement that destinations spawn the source's exact item

Those lines are deliberate. They keep later geospatial (Cesium) and public-goods (NLnet) work complementary instead of duplicated.
