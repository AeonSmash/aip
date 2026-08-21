# Unreal toolchain (M0)

Verified on this machine (2026-08-20 / 2026-08-21):

| Component | Status |
| --- | --- |
| Unreal Engine | **5.8.1** at `C:\Program Files\Epic Games\UE_5.8` |
| Visual Studio | Build Tools **2022** (17.14) with MSVC 14.44 |
| .NET Framework | **4.8.1 Developer Pack** (required by SwarmInterface / editor target) |
| Node.js | **v24.12.0** (for AIP TypeScript core) |

## Pass criteria

1. Open `AIPReference.uproject` (First Person C++ template, renamed).
2. Modules compile.
3. Press Play in the editor.

**CLI compile result:** `AIPReferenceEditor Win64 Development` — **Succeeded**.

## Project

- Path: [`AIPReference/`](AIPReference/)
- Template origin: `UE_5.8/Templates/TP_FirstPerson` (Shooter/Horror variants removed)
- Plugin: [`AIPReference/Plugins/AIP/`](AIPReference/Plugins/AIP/)

```bat
"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" AIPReferenceEditor Win64 Development -Project="...\AIPReference.uproject" -WaitMutex
```
