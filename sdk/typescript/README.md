# @aeonsmash/aip

TypeScript AIP Core: validate 0.1 envelopes and resolve destination-owned mappings.

## Setup

```bash
cd sdk/typescript
npm install
npm test
```

## CLI

```bash
npm run aip -- validate ../../examples/emberblade.aip.json
npm run aip -- map --destination unreal-fps ../../examples/emberblade.aip.json
npm run aip -- map --destination decentraland ../../examples/arena-sigil.aip.json
npm run aip -- exchange write-inbox ../../examples/emberblade.aip.json
npm run aip -- exchange list
```

Destinations: `unreal` | `unreal-fps` | `decentraland`
