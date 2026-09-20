"""Project and game asset paths."""

from __future__ import annotations

from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parents[1]
PARTS_DIR = PROJECT_ROOT / "parts"
ENEMIES_DIR = PROJECT_ROOT / "enemies"
GAME_ROOT = PROJECT_ROOT.parent
TEXTURES_DIR = GAME_ROOT / "resources" / "textures"
