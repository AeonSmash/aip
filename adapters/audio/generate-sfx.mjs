/**
 * Generate AIP demo SFX.
 * Tries ElevenLabs text-to-sound first (needs sound_generation on the key).
 * Falls back to a local analog/Rhodes synth so the demo is not silent.
 *
 * Usage: node adapters/audio/generate-sfx.mjs
 */
import { mkdirSync, readFileSync, writeFileSync, copyFileSync, existsSync } from 'fs'
import { dirname, join } from 'path'
import { fileURLToPath } from 'url'
import { execFileSync } from 'child_process'

const SR = 24000
const here = dirname(fileURLToPath(import.meta.url))
const grantsRoot = join(here, '..', '..', '..')
const envPaths = [
  join(grantsRoot, '.env'),
  join(grantsRoot, '..', 'first-light-cartridge', '.env')
]
const outDir = join(here, 'sfx')
const unrealDir = join(here, '..', 'unreal', 'AIPReference', 'Content', 'AIP', 'Audio')
const dclDir = join(grantsRoot, 'decentraland', 'sounds')

const clips = [
  {
    id: 'rifle',
    duration: 0.9,
    loop: false,
    influence: 0.5,
    text:
      'A single sci-fi sniper rifle shot. Tight percussive crack with a metallic mechanical snap, then a short decaying tail. Powerful but controlled and dry, close mic. No music, no voice, no long reverb, no explosion rumble, no ricochet whine. Video game rifle gunfire one-shot.'
  },
  {
    id: 'pistol',
    duration: 0.7,
    loop: false,
    influence: 0.45,
    text:
      'A small sci-fi pistol shot. Short muted analog synth pluck mixed with a soft mechanical click. Warm brass timbre. Dry, intimate, not loud. No explosion, no bass boom, no ricochet, no voice, no music melody. Video game gunfire one-shot.'
  },
  {
    id: 'linkbeam_pulse',
    duration: 0.85,
    loop: false,
    influence: 0.4,
    text:
      'A single round energy pulse launching, like a green plasma sphere leaving a barrel. Soft whoosh plus a warm analog Rhodes electric-piano attack. Sci-fi, not a gunshot, not explosive, not harsh. Short game sound effect. No voice.'
  },
  {
    id: 'linkbeam_link',
    duration: 6,
    loop: true,
    influence: 0.55,
    text:
      'Seamless looping sustain of a soft Fender Rhodes electric piano pad with a gentle analog oscillator hum. Warm major-seventh chord, slow chorus, quiet and lush. Continuous energy-link beam. No melody riff, no percussion, no voice, not loud, not shrill.'
  },
  {
    id: 'footstep',
    duration: 0.5,
    loop: false,
    influence: 0.35,
    text:
      'A single soft first-person footstep on smooth stone. Rubber sole, quiet, dry, close mic. Not heavy boots, not clacky high heels, no gravel crunch, no voice. Short game Foley.'
  },
  {
    id: 'terminal_rhodes',
    duration: 4.2,
    loop: false,
    influence: 0.5,
    text:
      'An extravagant but gentle Fender Rhodes electric piano activation sting. Lush C major ninth chord with slow tremolo and analog warmth. Ceremonial, cinematic, quiet, not annoying. No drums, no choir, no voice, no distortion.'
  }
]

function extractKey(text) {
  const named = text.match(/^\s*(?:ELEVENLABS_API_KEY|XI_API_KEY)\s*=\s*(.+)$/im)
  if (named) {
    return named[1].trim().replace(/^["']|["']$/g, '')
  }
  const labeled = text.match(/eleven\s*labs[^:\n]*:\s*(sk_[A-Za-z0-9]+)/i)
  if (labeled) {
    return labeled[1].trim()
  }
  const bare = text.match(/\b(sk_[A-Za-z0-9]+)\b/)
  return bare ? bare[1] : ''
}

function readKey() {
  for (const envPath of envPaths) {
    if (!existsSync(envPath)) {
      continue
    }
    const key = extractKey(readFileSync(envPath, 'utf8'))
    if (key) {
      return key
    }
  }
  return ''
}

function pcmToWav(pcm, sampleRate = SR, channels = 1, bits = 16) {
  const dataSize = pcm.length
  const header = Buffer.alloc(44)
  header.write('RIFF', 0)
  header.writeUInt32LE(36 + dataSize, 4)
  header.write('WAVE', 8)
  header.write('fmt ', 12)
  header.writeUInt32LE(16, 16)
  header.writeUInt16LE(1, 20)
  header.writeUInt16LE(channels, 22)
  header.writeUInt32LE(sampleRate, 24)
  header.writeUInt32LE((sampleRate * channels * bits) / 8, 28)
  header.writeUInt16LE((channels * bits) / 8, 32)
  header.writeUInt16LE(bits, 34)
  header.write('data', 36)
  header.writeUInt32LE(dataSize, 40)
  return Buffer.concat([header, pcm])
}

function readWavPcm(wav) {
  if (wav.length < 44 || wav.toString('ascii', 0, 4) !== 'RIFF') {
    return null
  }
  let offset = 12
  let fmt = null
  while (offset + 8 <= wav.length) {
    const id = wav.toString('ascii', offset, offset + 4)
    const size = wav.readUInt32LE(offset + 4)
    offset += 8
    if (id === 'fmt ' && size >= 16) {
      fmt = {
        channels: wav.readUInt16LE(offset + 2),
        sampleRate: wav.readUInt32LE(offset + 4),
        bits: wav.readUInt16LE(offset + 14)
      }
    } else if (id === 'data') {
      if (!fmt || fmt.bits !== 16) {
        return null
      }
      return { ...fmt, pcm: wav.subarray(offset, Math.min(offset + size, wav.length)) }
    }
    offset += size + (size & 1)
  }
  return null
}

/**
 * Generated one-shots come back clipped and padded with room tone. Trim the dead
 * tail so rapid fire does not smear, and pull the peak under full scale.
 */
function tidyOneShot(wav, { peak = 0.89, floorRms = 0.03, tailMs = 60, fadeMs = 6 } = {}) {
  const parsed = readWavPcm(wav)
  if (!parsed) {
    return wav
  }
  const { sampleRate, channels, pcm } = parsed
  const total = Math.floor(pcm.length / 2)
  if (total < 2) {
    return wav
  }

  const samples = new Float64Array(total)
  let max = 0
  for (let i = 0; i < total; i++) {
    samples[i] = pcm.readInt16LE(i * 2) / 32768
    max = Math.max(max, Math.abs(samples[i]))
  }

  const win = Math.max(1, Math.floor((sampleRate * channels * 20) / 1000))
  let end = total
  for (let start = total - win; start >= 0; start -= win) {
    let sum = 0
    for (let i = start; i < start + win && i < total; i++) {
      sum += samples[i] * samples[i]
    }
    if (Math.sqrt(sum / win) > floorRms) {
      end = Math.min(total, start + win + Math.floor((sampleRate * channels * tailMs) / 1000))
      break
    }
  }

  const gain = max > 0 ? peak / max : 1
  const fade = Math.max(1, Math.floor((sampleRate * channels * fadeMs) / 1000))
  const out = Buffer.alloc(end * 2)
  for (let i = 0; i < end; i++) {
    let v = samples[i] * gain
    if (i < fade) {
      v *= i / fade
    }
    if (end - i < fade) {
      v *= (end - i) / fade
    }
    out.writeInt16LE(Math.max(-32767, Math.min(32767, Math.round(v * 32767))), i * 2)
  }
  return pcmToWav(out, sampleRate, channels)
}

function samplesToPcm(samples) {
  const pcm = Buffer.alloc(samples.length * 2)
  for (let i = 0; i < samples.length; i++) {
    const v = Math.max(-1, Math.min(1, samples[i]))
    pcm.writeInt16LE(Math.round(v * 32767), i * 2)
  }
  return pcm
}

function envExp(t, attack, decay) {
  if (t < attack) {
    return t / attack
  }
  return Math.exp(-(t - attack) / decay)
}

function rhodesPartial(t, freq, amp) {
  const index = 2.2 * Math.exp(-t * 3.4)
  const mod = Math.sin(2 * Math.PI * freq * 14 * t) * index
  const tone = Math.sin(2 * Math.PI * freq * t + mod)
  const hammer = Math.sin(2 * Math.PI * freq * 2 * t) * Math.exp(-t * 18) * 0.12
  return amp * (tone + hammer)
}

function synthRifle(duration) {
  const n = Math.floor(SR * duration)
  const out = new Float64Array(n)
  for (let i = 0; i < n; i++) {
    const t = i / SR
    const crack = (Math.random() * 2 - 1) * Math.exp(-t * 90) * 0.42
    const body = Math.sin(2 * Math.PI * 132 * t) * envExp(t, 0.002, 0.05) * 0.3
    const snap = Math.sin(2 * Math.PI * 620 * t + Math.sin(2 * Math.PI * 1240 * t) * 0.6) * envExp(t, 0.001, 0.03) * 0.18
    const tail = (Math.random() * 2 - 1) * Math.exp(-t * 7) * 0.06
    out[i] = crack + body + snap + tail
  }
  return out
}

function synthPistol(duration) {
  const n = Math.floor(SR * duration)
  const out = new Float64Array(n)
  for (let i = 0; i < n; i++) {
    const t = i / SR
    const click = (Math.random() * 2 - 1) * Math.exp(-t * 55) * 0.18
    const brass = Math.sin(2 * Math.PI * 186 * t) * envExp(t, 0.004, 0.09) * 0.22
    const pluck = Math.sin(2 * Math.PI * 392 * t + Math.sin(2 * Math.PI * 784 * t) * 0.4) * envExp(t, 0.003, 0.07) * 0.2
    out[i] = click + brass + pluck
  }
  return out
}

function synthPulse(duration) {
  const n = Math.floor(SR * duration)
  const out = new Float64Array(n)
  for (let i = 0; i < n; i++) {
    const t = i / SR
    const freq = 420 - t * 260
    const whoosh = (Math.random() * 2 - 1) * Math.exp(-t * 6) * 0.08
    const body = rhodesPartial(t, freq, 0.22) * envExp(t, 0.01, 0.28)
    const low = Math.sin(2 * Math.PI * 92 * t) * envExp(t, 0.02, 0.2) * 0.12
    out[i] = whoosh + body + low
  }
  return out
}

function synthLink(duration) {
  const n = Math.floor(SR * duration)
  const out = new Float64Array(n)
  const chord = [174.61, 220.0, 261.63, 329.63]
  for (let i = 0; i < n; i++) {
    const t = i / SR
    const trem = 0.72 + 0.28 * Math.sin(2 * Math.PI * 0.35 * t)
    let s = 0
    for (const f of chord) {
      const chorus = f * (1 + 0.003 * Math.sin(2 * Math.PI * 0.15 * t))
      s += rhodesPartial(t, chorus, 0.09)
      s += Math.sin(2 * Math.PI * (chorus / 2) * t) * 0.04
    }
    const fade = Math.min(t / 0.4, (duration - t) / 0.4, 1)
    out[i] = s * trem * fade * 0.55
  }
  return out
}

function synthFootstep(duration) {
  const n = Math.floor(SR * duration)
  const out = new Float64Array(n)
  for (let i = 0; i < n; i++) {
    const t = i / SR
    const thud = Math.sin(2 * Math.PI * 68 * t) * envExp(t, 0.004, 0.07) * 0.28
    const scuff = (Math.random() * 2 - 1) * Math.exp(-t * 22) * 0.1
    out[i] = thud + scuff
  }
  return out
}

function synthTerminal(duration) {
  const n = Math.floor(SR * duration)
  const out = new Float64Array(n)
  const chord = [130.81, 164.81, 196.0, 246.94, 293.66]
  for (let i = 0; i < n; i++) {
    const t = i / SR
    const attack = envExp(t, 0.08, 1.6)
    const trem = 0.82 + 0.18 * Math.sin(2 * Math.PI * 4.5 * t)
    let s = 0
    for (let k = 0; k < chord.length; k++) {
      s += rhodesPartial(t, chord[k], 0.16 / (1 + k * 0.15))
    }
    out[i] = s * attack * trem * 0.7
  }
  return out
}

const synthById = {
  rifle: synthRifle,
  pistol: synthPistol,
  linkbeam_pulse: synthPulse,
  linkbeam_link: synthLink,
  footstep: synthFootstep,
  terminal_rhodes: synthTerminal
}

async function generateEleven(key, clip, outputFormat) {
  const url = `https://api.elevenlabs.io/v1/sound-generation?output_format=${outputFormat}`
  const res = await fetch(url, {
    method: 'POST',
    headers: {
      'xi-api-key': key,
      'Content-Type': 'application/json',
      Accept: 'application/octet-stream'
    },
    body: JSON.stringify({
      text: clip.text,
      model_id: 'eleven_text_to_sound_v2',
      duration_seconds: clip.duration,
      prompt_influence: clip.influence,
      loop: clip.loop
    })
  })
  if (!res.ok) {
    const errText = await res.text()
    const err = new Error(`${clip.id} ${outputFormat} HTTP ${res.status}: ${errText.slice(0, 400)}`)
    err.status = res.status
    err.body = errText
    throw err
  }
  return Buffer.from(await res.arrayBuffer())
}

function hasFfmpeg() {
  try {
    execFileSync('ffmpeg', ['-version'], { stdio: 'ignore' })
    return true
  } catch {
    return false
  }
}

function writeClipFiles(id, wav, mp3) {
  const wavPath = join(outDir, `${id}.wav`)
  writeFileSync(wavPath, wav)
  copyFileSync(wavPath, join(unrealDir, `${id}.wav`))
  copyFileSync(wavPath, join(dclDir, `${id}.wav`))
  if (mp3) {
    const mp3Path = join(outDir, `${id}.mp3`)
    writeFileSync(mp3Path, mp3)
    copyFileSync(mp3Path, join(dclDir, `${id}.mp3`))
  }
}

async function main() {
  mkdirSync(outDir, { recursive: true })
  mkdirSync(unrealDir, { recursive: true })
  mkdirSync(dclDir, { recursive: true })

  // Named ids regenerate just those clips, so refreshing one cue does not
  // re-bill or re-roll every other sound.
  const only = process.argv.slice(2)
  const selected = only.length ? clips.filter((c) => only.includes(c.id)) : clips
  if (!selected.length) {
    throw new Error(`No clips matched: ${only.join(', ')}`)
  }

  const key = readKey()
  const ffmpeg = hasFfmpeg()
  let source = 'local-synth'
  if (key) {
    try {
      process.stdout.write('Trying ElevenLabs sound_generation... ')
      await generateEleven(key, selected[0], 'mp3_44100_128')
      source = 'elevenlabs'
      console.log('ok')
    } catch (err) {
      const missing = String(err.body || err.message || '').includes('sound_generation')
      console.log(missing ? 'key lacks sound_generation, using local Rhodes/analog synth' : `failed (${err.message}), using local synth`)
    }
  } else {
    console.log('No ELEVENLABS_API_KEY, using local synth')
  }

  console.log(`Generating ${selected.length} clips via ${source} (ffmpeg=${ffmpeg})`)

  for (const clip of selected) {
    process.stdout.write(`  ${clip.id}... `)
    if (source === 'elevenlabs') {
      const mp3 = await generateEleven(key, clip, 'mp3_44100_128')
      let wav
      if (ffmpeg) {
        const mp3Path = join(outDir, `${clip.id}.mp3`)
        writeFileSync(mp3Path, mp3)
        const wavPath = join(outDir, `${clip.id}.wav`)
        execFileSync('ffmpeg', ['-y', '-i', mp3Path, '-ac', '1', '-ar', String(SR), wavPath], { stdio: 'ignore' })
        wav = readFileSync(wavPath)
      } else {
        const pcm = await generateEleven(key, clip, 'pcm_24000')
        wav = pcmToWav(pcm)
      }
      if (!clip.loop) {
        wav = tidyOneShot(wav)
      }
      writeClipFiles(clip.id, wav, mp3)
      console.log(`ok (${(wav.length / (2 * SR)).toFixed(2)}s)`)
      continue
    }

    const samples = synthById[clip.id](clip.duration)
    const wav = pcmToWav(samplesToPcm(samples))
    writeClipFiles(clip.id, wav, null)
    console.log(`ok (${wav.length} wav bytes)`)
  }
}

main().catch((err) => {
  console.error(err.message || err)
  process.exit(1)
})
