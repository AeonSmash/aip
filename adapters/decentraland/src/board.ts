import { BOARD_URL, SESSION, WRITE_KEY } from './config'
import { AipEnvelope } from './envelope'

export type BoardSnapshot = {
  session: string
  envelopes: AipEnvelope[]
}

export async function postEnvelope(envelope: AipEnvelope): Promise<boolean> {
  if (!BOARD_URL) {
    return false
  }
  try {
    const res = await fetch(`${BOARD_URL}/envelopes?session=${SESSION}`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'X-AIP-Write-Key': WRITE_KEY
      },
      body: JSON.stringify({ envelope })
    })
    return res.ok
  } catch {
    return false
  }
}

export async function fetchLatest(): Promise<AipEnvelope[]> {
  if (!BOARD_URL) {
    return []
  }
  try {
    const res = await fetch(`${BOARD_URL}/latest?session=${SESSION}`)
    if (!res.ok) {
      return []
    }
    const body = (await res.json()) as BoardSnapshot
    return Array.isArray(body.envelopes) ? body.envelopes : []
  } catch {
    return []
  }
}
