# Exchange contract

File-based transport for the AIP Unreal MVP. No sockets. No DirectoryWatcher.

## Layout

```text
exchange/
  inbox/     # foreign envelopes for Unreal to load
  outbox/    # envelopes Unreal writes for other destinations
```

## Filename rules

- Extension: `.aip.json`
- Basename: sanitized envelope `id` — replace anything outside `A-Za-z0-9._-` with `_`
  (Windows forbids `:` in filenames, so `aip:epochs:emberblade:01` → `aip_epochs_emberblade_01.aip.json`)
- Example: `aip:epochs:emberblade:01` → `aip_epochs_emberblade_01.aip.json`

## Inbox (Unreal inbound)

1. Author or obtain a valid 0.1 envelope.
2. Validate and copy with the CLI, **or** pull the web breaker (`aip switch serve` → POST `/pull`):

```bash
cd sdk/typescript
npm run aip -- validate ../../examples/main-breaker.aip.json
npm run aip -- exchange write-inbox ../../examples/main-breaker.aip.json
```

3. In Unreal, press the **Interact** key at the mid-field terminal (or call `LoadAipEnvelopeFromInbox`). Do **not** rely on automatic folder watching — `IDirectoryWatcher` is editor-only and breaks packaged builds.

## Outbox (Unreal outbound)

1. Unreal writes an achievement/sigil envelope to `exchange/outbox/` on terminal export or a demo kill.
2. Map it for another destination:

```bash
npm run aip -- validate ../../exchange/outbox/<file>.aip.json
npm run aip -- map --destination decentraland ../../exchange/outbox/<file>.aip.json
```

## Path resolution in Unreal

The plugin resolves the exchange root relative to the `.uproject` by walking up until it finds sibling `exchange/` at the AIP repo root (`aip/exchange`). Override with console var or default game config `AIP.ExchangeRoot` if needed.

## Non-goals

- Live sync between processes
- Network transport
- Atomic rename protocols beyond "write complete JSON file"
