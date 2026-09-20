"""Read/write .part files (v1)."""

from __future__ import annotations

from pathlib import Path

from src.models import EnemyStats, Part

PART_FORMAT_VERSION = "v1"


def load_part(path: Path) -> Part:
    lines = path.read_text(encoding="utf-8").splitlines()
    if not lines or lines[0] != PART_FORMAT_VERSION:
        raise ValueError(f"Unsupported or missing .part version in {path}")
    if len(lines) != 11:
        raise ValueError(f"Expected 11 lines in {path}, got {len(lines)}")

    return Part(
        name=lines[1],
        texture=lines[2],
        textureSizeX=int(lines[3]),
        textureSizeY=int(lines[4]),
        stats=EnemyStats(
            health=float(lines[5]),
            healthMult=float(lines[6]),
            speed=float(lines[7]),
            speedMult=float(lines[8]),
            action=float(lines[9]),
            actionMult=float(lines[10]),
        ),
    )


def save_part(part: Part, path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    content = "\n".join(
        [
            PART_FORMAT_VERSION,
            part.name,
            part.texture,
            str(part.textureSizeX),
            str(part.textureSizeY),
            _fmt(part.stats.health),
            _fmt(part.stats.healthMult),
            _fmt(part.stats.speed),
            _fmt(part.stats.speedMult),
            _fmt(part.stats.action),
            _fmt(part.stats.actionMult),
        ]
    )
    path.write_text(content + "\n", encoding="utf-8")


def part_path(parts_dir: Path, name: str) -> Path:
    return parts_dir / f"{name}.part"


def list_part_files(parts_dir: Path) -> list[Path]:
    if not parts_dir.is_dir():
        return []
    return sorted(parts_dir.glob("*.part"))


def load_all_parts(parts_dir: Path) -> dict[str, Part]:
    parts: dict[str, Part] = {}
    for path in list_part_files(parts_dir):
        part = load_part(path)
        parts[part.name] = part
    return parts


def _fmt(value: float) -> str:
    text = f"{value:g}"
    return text if text else "0"
