# AIP demo sound effects

ElevenLabs text-to-sound clips shared by Unreal and Decentraland.

| File | Use |
|---|---|
| `rifle` | Unreal starter rifle shot |
| `pistol` | DCL Box 1 click |
| `linkbeam_pulse` | LinkBeam LMB; DCL plaque click; Unreal terminal reveal |
| `linkbeam_link` | LinkBeam RMB hold (loop) |
| `footstep` | Walking in both worlds |
| `terminal_rhodes` | Unreal **E** on the terminal; DCL plaque appear |

Regenerate:

```bash
node adapters/audio/generate-sfx.mjs
node adapters/audio/generate-sfx.mjs rifle
```

Named ids regenerate only those clips.

The first-light key needs the **`sound_generation`** permission for ElevenLabs clips. Without it the script writes a local analog/Rhodes synth instead (still quiet, same filenames).
