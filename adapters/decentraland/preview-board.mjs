/**
 * AIP envelope board — demo bus for DCL, Unreal, and the CRT switch.
 * Not user accounts. Write keys stamp source.world. One current envelope per type;
 * a later POST of the same type replaces it.
 *
 *   POST /envelopes?session=demo   header X-AIP-Write-Key
 *   GET  /latest?session=demo
 *
 * Default keys: dcl-demo | unreal-demo | web-demo
 */
import fs from 'node:fs'
import http from 'node:http'
import path from 'node:path'
import { fileURLToPath } from 'node:url'
import { URL } from 'node:url'

const here = path.dirname(fileURLToPath(import.meta.url))
const SWITCH_HTML = path.resolve(here, '../aip/adapters/web/switch/index.html')

const PORT = Number(process.env.AIP_BOARD_PORT || 8788)

const KEYS = {
  [process.env.AIP_WRITE_KEY_DCL || 'dcl-demo']: {
    world: 'decentraland',
    app: 'aip-dcl-reference',
    type: 'signal.box'
  },
  [process.env.AIP_WRITE_KEY_UNREAL || 'unreal-demo']: {
    world: 'unreal',
    app: 'aip-unreal-reference',
    type: 'signal.terminal'
  },
  [process.env.AIP_WRITE_KEY_WEB || 'web-demo']: {
    world: 'web',
    app: 'aip-switch',
    type: 'signal.breaker'
  }
}

const ALLOWED = new Set(['signal.box', 'signal.terminal', 'signal.breaker'])

/** session -> type -> envelope */
const sessions = new Map()

function cors(res) {
  res.setHeader('Access-Control-Allow-Origin', '*')
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type, X-AIP-Write-Key')
  res.setHeader('Access-Control-Allow-Methods', 'GET,POST,OPTIONS')
}

function json(res, status, body) {
  cors(res)
  res.writeHead(status, { 'Content-Type': 'application/json; charset=utf-8' })
  res.end(JSON.stringify(body))
}

function envelopesOf(session) {
  const byType = sessions.get(session)
  return byType ? [...byType.values()] : []
}

function stampEnvelope(raw, identity) {
  const issuedAt = new Date().toISOString()
  const id = `aip:${identity.world}:${identity.type}:${issuedAt.replace(/[^0-9]/g, '')}-${Math.random().toString(36).slice(2, 8)}`
  return {
    aip: '0.1',
    kind: 'event',
    id,
    type: identity.type,
    label: typeof raw.label === 'string' && raw.label.length > 0 ? raw.label : identity.type,
    source: {
      world: identity.world,
      app: identity.app,
      scene: typeof raw.source?.scene === 'string' ? raw.source.scene : undefined,
      issuedAt
    },
    provenance: raw.provenance && typeof raw.provenance === 'object' ? raw.provenance : undefined,
    context: raw.context && typeof raw.context === 'object' ? raw.context : undefined,
    capabilities: Array.isArray(raw.capabilities) ? raw.capabilities : ['display', 'quest-flag'],
    rights: raw.rights && typeof raw.rights === 'object' ? raw.rights : { transferable: false, displayable: true }
  }
}

function validatePosted(raw, identity) {
  if (!raw || typeof raw !== 'object') return 'invalid envelope'
  if (raw.aip && raw.aip !== '0.1') return 'aip must be 0.1'
  if (raw.kind && raw.kind !== 'event') return 'kind must be event'
  if (raw.type && raw.type !== identity.type) return `this key may only post ${identity.type}`
  if (!ALLOWED.has(identity.type)) return 'type not allowlisted'
  return null
}

const server = http.createServer((req, res) => {
  if (!req.url) {
    json(res, 400, { error: 'no url' })
    return
  }
  const url = new URL(req.url, `http://127.0.0.1:${PORT}`)
  const session = url.searchParams.get('session') || 'demo'

  if (req.method === 'OPTIONS') {
    cors(res)
    res.writeHead(204)
    res.end()
    return
  }

  if (req.method === 'GET' && url.pathname === '/health') {
    json(res, 200, { ok: true, port: PORT })
    return
  }

  if (req.method === 'GET' && url.pathname === '/latest') {
    json(res, 200, { session, envelopes: envelopesOf(session) })
    return
  }

  if (req.method === 'GET' && (url.pathname === '/switch' || url.pathname === '/switch/')) {
    cors(res)
    try {
      const html = fs.readFileSync(SWITCH_HTML)
      res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' })
      res.end(html)
    } catch {
      json(res, 404, { error: 'switch page not found', path: SWITCH_HTML })
    }
    return
  }

  if (req.method === 'POST' && url.pathname === '/envelopes') {
    const key = req.headers['x-aip-write-key']
    const identity = typeof key === 'string' ? KEYS[key] : undefined
    if (!identity) {
      json(res, 401, { error: 'invalid or missing X-AIP-Write-Key' })
      return
    }

    let raw = ''
    req.on('data', (chunk) => {
      raw += chunk
    })
    req.on('end', () => {
      try {
        const parsed = JSON.parse(raw)
        const envelope = parsed.envelope || parsed
        const err = validatePosted(envelope, identity)
        if (err) {
          json(res, 400, { error: err })
          return
        }
        const stamped = stampEnvelope(envelope, identity)
        if (!sessions.has(session)) {
          sessions.set(session, new Map())
        }
        const byType = sessions.get(session)
        const replaced = byType.has(stamped.type)
        byType.set(stamped.type, stamped)
        console.log(`[${session}] ${stamped.type} ← ${stamped.source.world}${replaced ? ' (replaced)' : ''}`)
        json(res, 200, { ok: true, replaced, envelope: stamped })
      } catch {
        json(res, 400, { error: 'bad json' })
      }
    })
    return
  }

  json(res, 404, { error: 'not found' })
})

server.listen(PORT, '127.0.0.1', () => {
  console.log(`AIP envelope board http://127.0.0.1:${PORT}`)
  console.log('POST /envelopes?session=demo  header X-AIP-Write-Key: dcl-demo | unreal-demo | web-demo')
  console.log('GET  /latest?session=demo')
  console.log('GET  /switch  (local CRT)')
})
