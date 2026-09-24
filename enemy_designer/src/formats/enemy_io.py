"""Read/write .enemy files (v1 / v2 / v3)."""

from __future__ import annotations

from pathlib import Path

from src.models import Enemy, EnemyPartPlacement, MAX_ENEMY_PARTS, clamp_color_channel

ENEMY_FORMAT_VERSION = "v3"
_DEFAULT_COLOR = (255, 255, 255, 255)


def load_enemy(path: Path) -> Enemy:
    lines = path.read_text(encoding="utf-8").splitlines()
    if not lines:
        raise ValueError(f"Empty .enemy file: {path}")

    version = lines[0]
    if version == "v3":
        return _load_sized(path, lines, part_stride=9)
    if version == "v2":
        return _load_sized(path, lines, part_stride=5)
    if version == "v1":
        parts = _parse_parts(path, lines[1:], stride=5)
        return Enemy(name=path.stem, width=1.0, height=1.0, parts=parts)
    raise ValueError(f"Unsupported .enemy version '{version}' in {path}")


def _load_sized(path: Path, lines: list[str], *, part_stride: int) -> Enemy:
    if len(lines) < 3:
        raise ValueError(f"Truncated {lines[0]} .enemy file: {path}")
    width = float(lines[1])
    height = float(lines[2])
    parts = _parse_parts(path, lines[3:], stride=part_stride)
    return Enemy(name=path.stem, width=width, height=height, parts=parts)


def _parse_parts(path: Path, body: list[str], *, stride: int) -> list[EnemyPartPlacement]:
    if len(body) % stride != 0:
        raise ValueError(f"Expected groups of {stride} lines per part in {path}")

    parts: list[EnemyPartPlacement] = []
    for i in range(0, len(body), stride):
        chunk = body[i : i + stride]
        color = _DEFAULT_COLOR
        if stride >= 9:
            color = (
                clamp_color_channel(int(chunk[5])),
                clamp_color_channel(int(chunk[6])),
                clamp_color_channel(int(chunk[7])),
                clamp_color_channel(int(chunk[8])),
            )
        parts.append(
            EnemyPartPlacement(
                partName=chunk[0],
                offsetX=float(chunk[1]),
                offsetY=float(chunk[2]),
                offsetZ=float(chunk[3]),
                rotation=float(chunk[4]),
                colorR=color[0],
                colorG=color[1],
                colorB=color[2],
                colorA=color[3],
            )
        )
        if len(parts) > MAX_ENEMY_PARTS:
            raise ValueError(
                f"Enemy exceeds MAX_ENEMY_PARTS ({MAX_ENEMY_PARTS}) in {path}"
            )
    return parts


def save_enemy(enemy: Enemy, path: Path) -> None:
    if len(enemy.parts) > MAX_ENEMY_PARTS:
        raise ValueError(f"Enemy has more than {MAX_ENEMY_PARTS} parts")

    path.parent.mkdir(parents=True, exist_ok=True)
    lines = [
        ENEMY_FORMAT_VERSION,
        _fmt(enemy.width),
        _fmt(enemy.height),
    ]
    for part in enemy.parts:
        r, g, b, a = part.clamped_color()
        lines.extend(
            [
                part.partName,
                _fmt(part.offsetX),
                _fmt(part.offsetY),
                _fmt(part.offsetZ),
                _fmt(part.rotation),
                str(r),
                str(g),
                str(b),
                str(a),
            ]
        )
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def enemy_path(enemies_dir: Path, name: str) -> Path:
    return enemies_dir / f"{name}.enemy"


def list_enemy_files(enemies_dir: Path) -> list[Path]:
    if not enemies_dir.is_dir():
        return []
    return sorted(enemies_dir.glob("*.enemy"))


def load_all_enemies(enemies_dir: Path) -> dict[str, Enemy]:
    enemies: dict[str, Enemy] = {}
    for path in list_enemy_files(enemies_dir):
        enemy = load_enemy(path)
        enemies[enemy.name] = enemy
    return enemies


def _fmt(value: float) -> str:
    text = f"{value:g}"
    return text if text else "0"
