# Blender / FBX drop folder

Placeholder cubes ship in the Unreal reference. Drop original art here, then import in the editor to:

- `Content/AIP/Map/` — modular environment FBX
- `Content/AIP/Weapons/` — LinkBeam and CyanSniper viewmodels

## Export

- Format: **FBX**
- Units: centimeters (Blender unit scale 0.01, or meters with FBX scale 100)
- Apply All Transforms
- One object per mesh
- Weapon origin at the grip; Empty named `Muzzle` at the barrel
- Optional collision hulls named `UCX_MeshName`

Do not import UT2004 maps, meshes, or sounds.

`map/*.fbx` stays local: GitHub rejects blobs over 100MB (`aeonicMap.fbx` is ~104MB). Weapon FBX files are small enough to commit.
