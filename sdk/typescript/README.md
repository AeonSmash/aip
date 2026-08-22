# @aeonsmash/aip

TypeScript AIP Core **0.5.1**: validate 0.1 envelopes and resolve destination-owned mappings.

## Setup

```bash
cd sdk/typescript
npm install
npm test
```

## CLI

```bash
npm run aip -- validate ../../examples/main-breaker.aip.json
npm run aip -- map --destination unreal-fps ../../examples/main-breaker.aip.json
npm run aip -- map --destination decentraland ../../examples/arena-sigil.aip.json
npm run aip -- switch serve
npm run aip -- exchange write-inbox ../../examples/main-breaker.aip.json
npm run aip -- exchange list
```

Destinations: `unreal` | `unreal-fps` | `decentraland`
