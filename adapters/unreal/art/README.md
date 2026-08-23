# Blender / FBX drop folder

Guns in `weapons/` are wired as first-person viewmodels. Copy/import them in the
editor to `Content/AIP/Weapons/` (or run `weapons/import_weapon_fbx.py` from the
Output Log). Map pieces still go to `Content/AIP/Map/`.

Source files in this folder:

| FBX | Unreal mesh | Equipped as |
| --- | --- | --- |
| `main-SNIPERriffle.fbx` | `/Game/AIP/Weapons/LightningGun` + `LightningGun.obj` | Starter pistol (cyan sniper is unused) |
| `main-LINKgun.fbx` | `/Game/AIP/Weapons/LinkGun` + `LinkGun.obj` | LinkBeam after the CRT unlock |

Regenerate the OBJ files with `python extract_fbx_mesh.py` after a new Blender export.

## Export

- Format: **FBX**
- Units: centimeters (Blender unit scale 0.01, or meters with FBX scale 100)
- Apply All Transforms
- One object per mesh
- Weapon origin at the grip; Empty named `Muzzle` at the barrel
- Optional collision hulls named `UCX_MeshName`

Do not import UT2004 maps, meshes, or sounds.

`map/*.fbx` stays local: GitHub rejects blobs over 100MB (`aeonicMap.fbx` is ~104MB). Weapon FBX files are small enough to commit.
