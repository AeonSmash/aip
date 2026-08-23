"""Import Blender gun FBX into Content/AIP/Weapons.

Run from the Unreal editor (Output Log → Python) or:

  UnrealEditor-Cmd.exe AIPReference.uproject -unattended -nop4 ^
    -ExecutePythonScript=".../art/weapons/import_weapon_fbx.py"
"""

from pathlib import Path

import unreal

ART = Path(r"C:\AeonSmash\Main-Workspace\grants\aip\adapters\unreal\art\weapons")
DEST = "/Game/AIP/Weapons"

JOBS = (
    ("main-SNIPERriffle.fbx", "LightningGun"),
    ("main-LINKgun.fbx", "LinkGun"),
)


def import_fbx(src: Path, asset_name: str) -> None:
    task = unreal.AssetImportTask()
    task.filename = str(src)
    task.destination_path = DEST
    task.destination_name = asset_name
    task.replace_existing = True
    task.automated = True
    task.save = True

    options = unreal.FbxImportUI()
    options.import_mesh = True
    options.import_as_skeletal = False
    options.import_animations = False
    options.import_materials = True
    options.import_textures = True
    options.mesh_type_to_import = unreal.FBXImportType.FBXIT_STATIC_MESH
    options.static_mesh_import_data.combine_meshes = True
    options.static_mesh_import_data.auto_generate_collision = False
    options.static_mesh_import_data.normal_import_method = (
        unreal.FBXNormalImportMethod.FBXNIM_IMPORT_NORMALS
    )
    task.options = options

    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    unreal.log(f"AIP imported {src.name} -> {DEST}/{asset_name}")


def main() -> None:
    unreal.EditorAssetLibrary.make_directory(DEST)
    for filename, asset_name in JOBS:
        import_fbx(ART / filename, asset_name)


if __name__ == "__main__":
    main()
