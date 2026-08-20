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

Example: EPOCHS describes Emberblade as `artifact.weapon` with relative power `high`. An Unreal reference game can map that to a cosmetic emblem. A Decentraland scene can map a foreign Unreal sigil to a museum collectible. Neither destination reproduces the source's combat system.

## Engine-neutral core, runtime adapters

```text
                    ┌─────────────────────┐
                    │      AIP Core       │
                    │  envelope · schema  │
                    │  validation · docs  │
                    └──────────┬──────────┘
           ┌───────────────────┼───────────────────┐
           ▼                   ▼                   ▼
   Decentraland adapter   Unreal adapter     Other adapters
   (independent runtime)  (grant milestone)  (future)
```

The Unreal adapter is important because it makes the protocol visible in real-time 3D. It is not a requirement of the core.

Optional mesh references (`representation.glb`) can travel with an envelope. Appearance is supporting evidence, not the claim. The claim is that **meaning** can cross worlds that do not share coordinates, engines, databases, or rules.

## What 0.1 is not

- Not a finished standard
- Not a live network protocol
- Not a blockchain layer
- Not a requirement that destinations spawn the source's exact item

Those lines are deliberate. They keep later geospatial (Cesium) and public-goods (NLnet) work complementary instead of duplicated.
