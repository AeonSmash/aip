"""Extract triangle meshes from binary FBX into OBJ (stdlib only)."""

from __future__ import annotations

import struct
import zlib
from pathlib import Path

ART = Path(__file__).resolve().parent
JOBS = (
    ("main-SNIPERriffle.fbx", "LightningGun.obj"),
    ("main-LINKgun.fbx", "LinkGun.obj"),
)


def _read(f, fmt: str):
    data = f.read(struct.calcsize(fmt))
    if len(data) < struct.calcsize(fmt):
        raise EOFError
    return struct.unpack(fmt, data)


def _read_array(f, elem_fmt: str, elem_size: int):
    count, encoding, comp_len = _read(f, "<III")
    raw = f.read(comp_len)
    if encoding == 1:
        raw = zlib.decompress(raw)
    elif encoding != 0:
        return []
    if len(raw) < count * elem_size:
        return []
    return list(struct.unpack("<" + elem_fmt * count, raw[: count * elem_size]))


def _read_property(f):
    kind = f.read(1)
    if not kind:
        raise EOFError
    t = kind.decode("ascii")
    if t == "Y":
        return _read(f, "<h")[0]
    if t == "C":
        return f.read(1)
    if t in "I":
        return _read(f, "<i")[0]
    if t == "F":
        return _read(f, "<f")[0]
    if t == "D":
        return _read(f, "<d")[0]
    if t == "L":
        return _read(f, "<q")[0]
    if t == "R":
        n = _read(f, "<I")[0]
        return f.read(n)
    if t == "S":
        n = _read(f, "<I")[0]
        return f.read(n)
    if t == "b":
        return _read_array(f, "?", 1)
    if t == "c":
        return _read_array(f, "B", 1)
    if t == "i":
        return _read_array(f, "i", 4)
    if t == "f":
        return _read_array(f, "f", 4)
    if t == "d":
        return _read_array(f, "d", 8)
    if t == "l":
        return _read_array(f, "q", 8)
    raise ValueError(f"unknown fbx property {t!r}")


def _read_node(f, version: int):
    if version >= 7500:
        end_offset, num_props, prop_len = _read(f, "<QQQ")
        null_size = 25
    else:
        end_offset, num_props, prop_len = _read(f, "<III")
        null_size = 13
    if end_offset == 0:
        return None
    name_len = _read(f, "<B")[0]
    name = f.read(name_len).decode("ascii", "replace")
    props = [_read_property(f) for _ in range(num_props)]
    children = []
    while f.tell() < end_offset:
        child = _read_node(f, version)
        if child is None:
            break
        children.append(child)
    f.seek(end_offset)
    return name, props, children


def _walk(node, visitor):
    if node is None:
        return
    visitor(node)
    for child in node[2]:
        _walk(child, visitor)


def extract_mesh(path: Path) -> tuple[list[tuple[float, float, float]], list[tuple[int, int, int]]]:
    with path.open("rb") as f:
        header = f.read(21)
        if not header.startswith(b"Kaydara FBX Binary"):
            raise ValueError(f"{path.name} is not a binary FBX")
        f.read(2)
        version = _read(f, "<I")[0]
        print(f"{path.name} fbx version {version}")
        nodes = []
        while True:
            node = _read_node(f, version)
            if node is None:
                break
            nodes.append(node)

    verts: list[float] = []
    indices: list[int] = []

    def visit(node):
        nonlocal verts, indices
        name, props, children = node
        if name != "Geometry":
            return
        child_map = {c[0]: c for c in children}
        v = child_map.get("Vertices")
        p = child_map.get("PolygonVertexIndex")
        if not v or not p:
            return
        vdata = v[1][0] if v[1] else []
        pdata = p[1][0] if p[1] else []
        if len(vdata) >= 9 and len(pdata) >= 3:
            verts = [float(x) for x in vdata]
            indices = [int(x) for x in pdata]

    for n in nodes:
        _walk(n, visit)

    if len(verts) < 9 or len(indices) < 3:
        raise ValueError(f"no mesh in {path.name}")

    points = [(verts[i], verts[i + 1], verts[i + 2]) for i in range(0, len(verts) - 2, 3)]
    # FBX Y-up -> Unreal Z-up: (x, y, z) -> (x, z, y)
    points = [(p[0], p[2], p[1]) for p in points]

    tris: list[tuple[int, int, int]] = []
    face: list[int] = []
    for idx in indices:
        if idx < 0:
            face.append(-idx - 1)
            if len(face) >= 3:
                a = face[0]
                for i in range(1, len(face) - 1):
                    tris.append((a, face[i], face[i + 1]))
            face = []
        else:
            face.append(idx)

    xs = [p[0] for p in points]
    ys = [p[1] for p in points]
    zs = [p[2] for p in points]
    cx = (min(xs) + max(xs)) * 0.5
    cy = (min(ys) + max(ys)) * 0.5
    cz = min(zs)
    size = max(max(xs) - min(xs), max(ys) - min(ys), max(zs) - min(zs), 0.001)
    # First-person viewmodel ~28cm long.
    scale = 28.0 / size
    points = [((p[0] - cx) * scale, (p[1] - cy) * scale, (p[2] - cz) * scale) for p in points]
    return points, tris


def write_obj(out: Path, points, tris) -> None:
    lines = ["# AIP viewmodel", f"# verts {len(points)} tris {len(tris)}"]
    for x, y, z in points:
        lines.append(f"v {x:.6f} {y:.6f} {z:.6f}")
    lines.append("vt 0.0 0.0")
    for a, b, c in tris:
        lines.append(f"f {a + 1}/1 {b + 1}/1 {c + 1}/1")
    out.write_text("\n".join(lines) + "\n", encoding="utf-8")


def main() -> None:
    for src_name, obj_name in JOBS:
        points, tris = extract_mesh(ART / src_name)
        write_obj(ART / obj_name, points, tris)
        print(f"{src_name} -> art/weapons/{obj_name} ({len(points)} verts, {len(tris)} tris)")


if __name__ == "__main__":
    main()
