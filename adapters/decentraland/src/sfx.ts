import { AudioSource, engine, Transform } from '@dcl/sdk/ecs'
import { Vector3 } from '@dcl/sdk/math'

/** Same clip names as Unreal Content/AIP/Audio. */
export const SFX = {
  pistol: 'sounds/pistol.mp3',
  pulse: 'sounds/linkbeam_pulse.mp3',
  link: 'sounds/linkbeam_link.mp3',
  footstep: 'sounds/footstep.mp3',
  terminal: 'sounds/terminal_rhodes.mp3'
}

const ANCHOR = Vector3.create(8, 1, 8)

let oneShot: ReturnType<typeof engine.addEntity> | null = null
let steps: ReturnType<typeof engine.addEntity> | null = null
let lastPos: Vector3 | null = null
let stepCooldown = 0

function ensure(existing: ReturnType<typeof engine.addEntity> | null) {
  if (existing) {
    return existing
  }
  const entity = engine.addEntity()
  Transform.create(entity, { position: ANCHOR })
  return entity
}

export function playSfx(url: string, volume = 0.55) {
  oneShot = ensure(oneShot)
  if (!AudioSource.has(oneShot)) {
    AudioSource.create(oneShot, {
      audioClipUrl: url,
      playing: true,
      loop: false,
      global: true,
      volume
    })
    return
  }
  const src = AudioSource.getMutable(oneShot)
  src.audioClipUrl = url
  src.loop = false
  src.global = true
  src.volume = volume
  AudioSource.playSound(oneShot, url, true)
}

export function tickFootsteps(dt: number) {
  stepCooldown -= dt
  const player = engine.PlayerEntity
  const transform = Transform.getOrNull(player)
  if (!transform) {
    return
  }

  const pos = transform.position
  if (!lastPos) {
    lastPos = Vector3.create(pos.x, pos.y, pos.z)
    return
  }

  const dx = pos.x - lastPos.x
  const dz = pos.z - lastPos.z
  const moved = Math.sqrt(dx * dx + dz * dz)
  lastPos = Vector3.create(pos.x, pos.y, pos.z)
  if (moved < 0.55 || stepCooldown > 0) {
    return
  }

  stepCooldown = 0.38
  steps = ensure(steps)
  if (!AudioSource.has(steps)) {
    AudioSource.create(steps, {
      audioClipUrl: SFX.footstep,
      playing: true,
      loop: false,
      global: true,
      volume: 0.2
    })
    return
  }
  AudioSource.playSound(steps, SFX.footstep, true)
  AudioSource.getMutable(steps).volume = 0.2
  AudioSource.getMutable(steps).global = true
}
