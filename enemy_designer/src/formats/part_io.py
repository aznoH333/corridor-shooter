"""Read/write .part files (v1). Placeholder — not implemented yet."""

from __future__ import annotations

from pathlib import Path

from src.models import Part


def load_part(path: Path) -> Part:
    raise NotImplementedError("load_part is not implemented yet")


def save_part(part: Part, path: Path) -> None:
    raise NotImplementedError("save_part is not implemented yet")
