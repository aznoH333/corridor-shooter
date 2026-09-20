"""combineStats — must stay bit-for-bit aligned with ../src/enemies.c."""

from __future__ import annotations

from src.models import Enemy, EnemyPartPlacement, EnemyStats, Part


def combine_stats(first: EnemyStats, second: EnemyStats) -> EnemyStats:
    """Match C combineStats(first, second)."""
    return EnemyStats(
        health=(first.health + second.health) * second.healthMult,
        healthMult=0.0,
        speed=(first.speed + second.speed) * second.speedMult,
        speedMult=0.0,
        action=(first.action + second.action) * second.actionMult,
        actionMult=0.0,
    )


def fold_part_stats(parts: list[Part]) -> EnemyStats:
    """Fold part stats in list order, starting from zero."""
    acc = EnemyStats()
    for part in parts:
        acc = combine_stats(acc, part.stats)
    return acc


def fold_enemy_stats(
    placements: list[EnemyPartPlacement],
    library: dict[str, Part],
) -> EnemyStats:
    """Fold placed-part stats in list order using the part library."""
    resolved: list[Part] = []
    for placement in placements:
        part = library.get(placement.partName)
        if part is not None:
            resolved.append(part)
    return fold_part_stats(resolved)


def fold_enemy(enemy: Enemy, library: dict[str, Part]) -> EnemyStats:
    return fold_enemy_stats(enemy.parts, library)
