"""Read/write .enemy files (v1 / v2)."""

from __future__ import annotations

from pathlib import Path

from src.models import Enemy, EnemyPartPlacement, MAX_ENEMY_PARTS

ENEMY_FORMAT_VERSION = "v2"


def load_enemy(path: Path) -> Enemy:
    lines = path.read_text(encoding="utf-8").splitlines()
    if not lines:
        raise ValueError(f"Empty .enemy file: {path}")

    version = lines[0]
    if version == "v2":
        return _load_v2(path, lines)
    if version == "v1":
        return _load_v1(path, lines)
    raise ValueError(f"Unsupported .enemy version '{version}' in {path}")


def _load_v2(path: Path, lines: list[str]) -> Enemy:
    if len(lines) < 3:
        raise ValueError(f"Truncated v2 .enemy file: {path}")
    width = float(lines[1])
    height = float(lines[2])
    parts = _parse_parts(path, lines[3:])
    return Enemy(name=path.stem, width=width, height=height, parts=parts)


def _load_v1(path: Path, lines: list[str]) -> Enemy:
    parts = _parse_parts(path, lines[1:])
    return Enemy(name=path.stem, width=1.0, height=1.0, parts=parts)


def _parse_parts(path: Path, body: list[str]) -> list[EnemyPartPlacement]:
    if len(body) % 5 != 0:
        raise ValueError(f"Expected groups of 5 lines per part in {path}")

    parts: list[EnemyPartPlacement] = []
    for i in range(0, len(body), 5):
        parts.append(
            EnemyPartPlacement(
                partName=body[i],
                offsetX=float(body[i + 1]),
                offsetY=float(body[i + 2]),
                offsetZ=float(body[i + 3]),
                rotation=float(body[i + 4]),
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
        lines.extend(
            [
                part.partName,
                _fmt(part.offsetX),
                _fmt(part.offsetY),
                _fmt(part.offsetZ),
                _fmt(part.rotation),
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
