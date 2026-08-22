# Aeonic Interoperability Protocol (AIP)

An open semantic interoperability protocol for independent games and real-time 3D worlds.

[Explainer video](https://youtu.be/3D2cgt-ajlE) · [Public draft](#public-draft-01)

A source world can describe an object, event, or identity. A destination world decides what that description means locally. Worlds do not share a database, engine, economy, or blockchain.

```text
Source World
object / event / identity
        ↓
AIP semantic envelope
meaning + provenance + relative context + capabilities
        ↓
Destination World
local interpretation
```

## Why this exists

Most “interoperability” copies files. Copying a GLB into another engine does not tell that engine whether the object is a weapon, a quest flag, a collectible, or a cosmetic.

AIP addresses a different problem: **portable meaning**.

- A powerful weapon in one RPG can arrive in another application as context, not as foreign damage numbers.
- The receiving game remains responsible for whether that object becomes a functional weapon, a balanced local equivalent, a cosmetic emblem, a collectible, or nothing.
- Participating worlds keep their own rendering, rules, security model, and data stores.

The architectural test is simple: **AIP should communicate meaning without requiring participating worlds to surrender control.**

## Public draft (0.1)

This repository is the public presentation of AIP. It is intentionally small. Internal design work continues separately.

| File | What it is |
| --- | --- |
| [`schemas/aip-envelope-0.1.schema.json`](schemas/aip-envelope-0.1.schema.json) | Draft envelope schema |
| [`examples/emberblade.aip.json`](examples/emberblade.aip.json) | Example object from an independent runtime |
| [`examples/arena-sigil.aip.json`](examples/arena-sigil.aip.json) | Example event from the Unreal reference |
| [`examples/unreal-local-mapping.json`](examples/unreal-local-mapping.json) | Example: how an Unreal destination *might* interpret that object |
| [`examples/decentraland-local-mapping.json`](examples/decentraland-local-mapping.json) | Example: how a Decentraland destination *might* interpret a foreign object |
| [`mappings/unreal-fps.json`](mappings/unreal-fps.json) | Destination-owned FPS rules (CyanSniper unlock) |
| [`sdk/typescript/`](sdk/typescript/) | Validator, mapping resolver, CLI, tests |
| [`exchange/`](exchange/) | File-drop inbox/outbox transport |
| [`adapters/unreal/`](adapters/unreal/) | UE 5.8 First Person reference + AIP plugin |
| [`docs/architecture.md`](docs/architecture.md) | Destination sovereignty and engine-neutral adapters |
| [`explainer/index.html`](explainer/index.html) | 90-second visual walkthrough |
| [`CHANGELOG.md`](CHANGELOG.md) | Public presentation history |

Watch the recorded explainer: https://youtu.be/3D2cgt-ajlE

### Quick CLI loop

```bash
cd sdk/typescript && npm install && npm test
npm run aip -- exchange write-inbox ../../examples/emberblade.aip.json
npm run aip -- map --destination unreal-fps ../../examples/emberblade.aip.json
npm run aip -- map --destination decentraland ../../examples/arena-sigil.aip.json
```

Then open [`adapters/unreal/AIPReference/AIPReference.uproject`](adapters/unreal/AIPReference/AIPReference.uproject), Play. LinkBeam is the starter gun. Press **E** at the terminal to unlock CyanSniper from Emberblade, **F** to export.
## Architecture

```mermaid
flowchart LR
  S["Any source world"]
  E["AIP envelope<br/>type · provenance · relative power · capabilities · rights"]
  D1["Destination A<br/>local interpretation"]
  D2["Destination B<br/>local interpretation"]

  S -->|describe| E
  E -->|interpret| D1
  E -->|interpret| D2
```

AIP does not require coordinate systems, gameplay stats, or item databases to become identical. It carries enough semantic context for each destination to decide.

Adapters are optional. The protocol stays engine-neutral:

```text
Independent runtime A
        ↕
     AIP Core
        ↕
Independent runtime B
```

No particular engine or world is a requirement of AIP Core. Unreal, Decentraland, web/Three.js, and other runtimes are example adapters only. They are useful for demonstrations. They are not part of the core.

## Current status

**Prototype / in development — playable one-tower Unreal reference (presentation 0.4.0).**

- TypeScript core validates and maps envelopes (`npm test` green); `@aeonsmash/aip` **0.4.0**
- File exchange contract under `exchange/`
- Unreal 5.8 First Person reference (PIE): LinkBeam + AIP-unlocked CyanSniper, core tower, NPC waves
- Bidirectional demo path: inbox weapon → Unreal sniper unlock; Unreal sigil → Decentraland museum mapping via CLI

Existing AeonSmash work is how the problem was found, not a dependency of the protocol.

AIP does **not** require:

- a shared game database
- a mandatory blockchain
- a common economy
- a single engine
- Decentraland, Unreal, or any other specific platform

## Roadmap

1. Freeze a tiny public envelope and examples *(this repo)*
2. A real-time 3D adapter / SDK and a small reference level *(Unreal MVP — playable in PIE)*
3. Bidirectional demonstration between two independent runtimes *(file transport + CLI peer)*
4. Validation, diagnostics, and conformance fixtures *(TypeScript tests — started)*
5. Documentation and sample integrations other developers can run

## License

MIT. See [LICENSE](LICENSE).

## Maintainer

[AeonSmash](https://github.com/AeonSmash) · [Decentraland Studios](https://studios.decentraland.org/profile/aeonsmash) · [aeonsmash.com](https://aeonsmash.com/)
