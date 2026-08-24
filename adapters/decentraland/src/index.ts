import {
  Billboard,
  BillboardMode,
  engine,
  Entity,
  executeTask,
  InputAction,
  Material,
  MeshCollider,
  MeshRenderer,
  pointerEventsSystem,
  TextAlignMode,
  TextShape,
  Transform,
  VisibilityComponent
} from '@dcl/sdk/ecs'
import { getPlayer } from '@dcl/sdk/players'
import { Color4, Quaternion, Vector3 } from '@dcl/sdk/math'
import { openExternalUrl } from '~system/RestrictedActions'
import { fetchLatest, postEnvelope } from './board'
import { POLL_SECONDS, SWITCH_URL } from './config'
import { isTerminalSignal, makeBoxEnvelope } from './envelope'
import { playSfx, SFX, tickFootsteps } from './sfx'

type Phase = 'idle' | 'emitted' | 'terminal'

const PLAQUE = {
  terminal: 'Throw the switch at\naeonsmash.com/#aip-switch\nClick this plaque to open it.'
}

export function main() {
  const floor = engine.addEntity()
  Transform.create(floor, {
    position: Vector3.create(8, 0, 8),
    scale: Vector3.create(16, 0.08, 16)
  })
  MeshRenderer.setBox(floor)
  Material.setPbrMaterial(floor, {
    albedoColor: Color4.create(0.05, 0.07, 0.09, 1)
  })

  const title = engine.addEntity()
  Transform.create(title, {
    position: Vector3.create(8, 3.4, 8)
  })
  TextShape.create(title, {
    text: 'AIP  //  DECENTRALAND',
    fontSize: 3,
    textAlign: TextAlignMode.TAM_MIDDLE_CENTER,
    textColor: Color4.create(0.91, 0.65, 0.29, 1)
  })
  Billboard.create(title, { billboardMode: BillboardMode.BM_Y })

  const box = engine.addEntity()
  Transform.create(box, {
    position: Vector3.create(8, 0.7, 8),
    scale: Vector3.create(1.2, 1.2, 1.2)
  })
  MeshRenderer.setBox(box)
  MeshCollider.setBox(box)
  paintBox(box, false)

  const boxLabel = engine.addEntity()
  Transform.create(boxLabel, {
    position: Vector3.create(8, 1.7, 8)
  })
  TextShape.create(boxLabel, {
    text: 'BOX 1',
    fontSize: 2,
    textAlign: TextAlignMode.TAM_MIDDLE_CENTER,
    textColor: Color4.create(0.3, 0.76, 0.85, 1)
  })
  Billboard.create(boxLabel, { billboardMode: BillboardMode.BM_Y })

  const plaque = engine.addEntity()
  Transform.create(plaque, {
    position: Vector3.create(3.2, 1.6, 8),
    rotation: Quaternion.fromEulerDegrees(0, -90, 0),
    scale: Vector3.create(0.001, 0.001, 0.001)
  })
  MeshRenderer.setPlane(plaque)
  MeshCollider.setPlane(plaque)
  Material.setPbrMaterial(plaque, {
    albedoColor: Color4.create(0.08, 0.1, 0.12, 1)
  })
  VisibilityComponent.create(plaque, { visible: false })

  const plaqueText = engine.addEntity()
  Transform.create(plaqueText, {
    position: Vector3.create(3.35, 1.6, 8),
    rotation: Quaternion.fromEulerDegrees(0, -90, 0),
    scale: Vector3.create(0.001, 0.001, 0.001)
  })
  TextShape.create(plaqueText, {
    text: '',
    fontSize: 1.15,
    textAlign: TextAlignMode.TAM_MIDDLE_CENTER,
    textColor: Color4.create(0.78, 0.97, 0.83, 1),
    width: 2.2,
    height: 1.4,
    textWrapping: true
  })
  VisibilityComponent.create(plaqueText, { visible: false })

  let phase: Phase = 'idle'
  let posted = false

  const setPhase = (next: Phase) => {
    if (phase === next) {
      return
    }
    phase = next
    paintBox(box, next !== 'idle')
    const showPlaque = next === 'terminal'
    VisibilityComponent.getMutable(plaque).visible = showPlaque
    VisibilityComponent.getMutable(plaqueText).visible = showPlaque
    Transform.getMutable(plaque).scale = showPlaque
      ? Vector3.create(2.4, 1.5, 1)
      : Vector3.create(0.001, 0.001, 0.001)
    Transform.getMutable(plaqueText).scale = showPlaque
      ? Vector3.create(1, 1, 1)
      : Vector3.create(0.001, 0.001, 0.001)
    TextShape.getMutable(plaqueText).text = showPlaque ? PLAQUE.terminal : ''
    if (next === 'terminal') {
      playSfx(SFX.terminal, 0.5)
    }
  }

  pointerEventsSystem.onPointerDown(
    {
      entity: box,
      opts: { button: InputAction.IA_POINTER, hoverText: 'Emit signal.box' }
    },
    () => {
      const player = getPlayer()
      const who = player?.userId ?? 'operator:anonymous'
      const envelope = makeBoxEnvelope(who)
      posted = true
      setPhase('emitted')
      playSfx(SFX.pistol, 0.45)
      executeTask(async () => {
        const ok = await postEnvelope(envelope)
        if (!ok) {
          TextShape.getMutable(boxLabel).text = 'BOX 1\n(board offline)'
        }
      })
    }
  )

  pointerEventsSystem.onPointerDown(
    {
      entity: plaque,
      opts: { button: InputAction.IA_POINTER, hoverText: 'Throw the switch at aeonsmash.com' }
    },
    () => {
      if (phase !== 'terminal') {
        return
      }
      playSfx(SFX.pulse, 0.5)
      void openExternalUrl({ url: SWITCH_URL })
    }
  )

  let timer = 0
  let seenIds: Set<string> | null = null
  engine.addSystem((dt) => {
    tickFootsteps(dt)
    timer += dt
    if (timer < POLL_SECONDS) {
      return
    }
    timer = 0
    executeTask(async () => {
      const envelopes = await fetchLatest()
      if (envelopes === null) {
        return
      }
      if (seenIds === null) {
        seenIds = new Set(envelopes.map((env) => env.id).filter(Boolean))
        return
      }
      for (const env of envelopes) {
        if (!env.id || seenIds.has(env.id)) {
          continue
        }
        seenIds.add(env.id)
        if (isTerminalSignal(env)) {
          setPhase('terminal')
          return
        }
      }
      if (posted && phase === 'idle') {
        setPhase('emitted')
      }
    })
  })
}

function paintBox(entity: Entity, armed: boolean) {
  Material.setPbrMaterial(entity, {
    albedoColor: armed ? Color4.create(0.91, 0.65, 0.29, 1) : Color4.create(0.18, 0.55, 0.62, 1),
    emissiveColor: armed ? Color4.create(0.91, 0.65, 0.29, 1) : Color4.create(0.12, 0.4, 0.48, 1),
    emissiveIntensity: armed ? 2.4 : 1.1
  })
}
