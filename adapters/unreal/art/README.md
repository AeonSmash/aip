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

The first-person viewmodel is grey until the FBX is imported as a static mesh.
`LightningGun.obj` has no UVs. The sniper textures live in
`weapons/main-SNIPERriffle.fbm/` (`Baked_BaseColor.png`, `normal.png`).
In the editor Output Log:

```
py ".../art/weapons/import_weapon_fbx.py"
```

Play then loads `/Game/AIP/Weapons/LightningGun` and hides the grey OBJ.

## Export

- Format: **FBX**
- Units: centimeters (Blender unit scale 0.01, or meters with FBX scale 100)
- Apply All Transforms
- One object per mesh
- Weapon origin at the grip; Empty named `Muzzle` at the barrel
- Barrel down Unreal **+X**. The muzzle is measured from the mesh's +X end, so a
  gun exported backwards fires out of its stock (`extract_fbx_mesh.py` yaws the
  sniper 180 because its FBX points -X)
- Optional collision hulls named `UCX_MeshName`

Do not import UT2004 maps, meshes, or sounds.

`map/*.fbx` stays local: GitHub rejects blobs over 100MB (`aeonicMap.fbx` is ~104MB). Weapon FBX files are small enough to commit.
