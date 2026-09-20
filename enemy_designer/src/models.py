"""Domain models mirroring game EnemyStats / parts / enemies."""

from __future__ import annotations

from dataclasses import dataclass, field


@dataclass
class EnemyStats:
    health: float = 0.0
    healthMult: float = 0.0
    speed: float = 0.0
    speedMult: float = 0.0
    action: float = 0.0
    actionMult: float = 0.0


def default_part_stats() -> EnemyStats:
    """Default stats for newly created parts."""
    return EnemyStats(
        health=0.0,
        healthMult=1.0,
        speed=0.0,
        speedMult=1.0,
        action=0.0,
        actionMult=1.0,
    )


@dataclass
class Part:
    """EnemyPartDefinition + tool-only name."""

    name: str = ""
    texture: str = ""
    textureSizeX: int = 0
    textureSizeY: int = 0
    stats: EnemyStats = field(default_factory=default_part_stats)


@dataclass
class EnemyPartPlacement:
    """Placed part on an enemy (EnemyPart in C)."""

    partName: str = ""
    offsetX: float = 0.0
    offsetY: float = 0.0
    offsetZ: float = 0.0
    rotation: float = 0.0  # radians in files


# Match ../src/enemies.h
MAX_ENEMY_PARTS = 32

# Part offsets are in pixels; enemy width/height are in game units.
PIXELS_PER_GAME_UNIT = 32


@dataclass
class Enemy:
    """Enemy composition. Filename stem is the id."""

    name: str = ""
    width: float = 1.0  # game units
    height: float = 1.0  # game units
    parts: list[EnemyPartPlacement] = field(default_factory=list)
