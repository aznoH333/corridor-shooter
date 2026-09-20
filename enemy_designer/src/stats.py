"""combineStats — must stay bit-for-bit aligned with ../src/enemies.c."""

from __future__ import annotations

from src.models import EnemyStats, Part


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
