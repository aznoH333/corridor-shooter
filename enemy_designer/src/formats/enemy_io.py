"""Read/write .enemy files (v1). Placeholder — not implemented yet."""

from __future__ import annotations

from pathlib import Path

from src.models import Enemy


def load_enemy(path: Path) -> Enemy:
    raise NotImplementedError("load_enemy is not implemented yet")


def save_enemy(enemy: Enemy, path: Path) -> None:
    raise NotImplementedError("save_enemy is not implemented yet")
