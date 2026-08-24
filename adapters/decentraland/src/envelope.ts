export type AipEnvelope = {
  aip: '0.1'
  kind: 'object' | 'event' | 'identity'
  id: string
  type: string
  label: string
  source: {
    world: string
    app: string
    scene?: string
    issuedAt?: string
  }
  provenance?: {
    origin?: string
    discoveredBy?: string
    history?: string[]
  }
  context?: {
    power?: {
      scale: 'relative'
      tier: 'low' | 'mid' | 'high' | 'exceptional'
      basis?: string
    }
  }
  capabilities?: string[]
  rights?: {
    owner?: string
    transferable?: boolean
    displayable?: boolean
  }
}

export function makeBoxEnvelope(discoveredBy: string): AipEnvelope {
  const issuedAt = new Date().toISOString()
  return {
    aip: '0.1',
    kind: 'event',
    id: `aip:dcl:box-1:${issuedAt.replace(/[^0-9]/g, '')}-${Math.random().toString(36).slice(2, 8)}`,
    type: 'signal.box',
    label: 'Box 1',
    source: {
      world: 'decentraland',
      app: 'aip-dcl-reference',
      scene: 'box-1',
      issuedAt
    },
    provenance: {
      origin: 'AIP Decentraland Box 1',
      discoveredBy,
      history: ['operator clicked Box 1']
    },
    context: {
      power: {
        scale: 'relative',
        tier: 'mid',
        basis: 'dcl box local signal'
      }
    },
    capabilities: ['display', 'quest-flag'],
    rights: {
      owner: discoveredBy,
      transferable: false,
      displayable: true
    }
  }
}

export function isTerminalSignal(env: AipEnvelope): boolean {
  return env.type === 'signal.terminal' && env.source.world === 'unreal'
}
