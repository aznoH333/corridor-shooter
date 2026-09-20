"""Enemy Designer tab UI. Placeholder layout for now."""

from __future__ import annotations

import dearpygui.dearpygui as dpg


def build_enemy_designer(*, parent: int | str) -> None:
    with dpg.group(parent=parent):
        dpg.add_text("Enemy Designer")
        dpg.add_separator()
        dpg.add_text("Placeholder — canvas, part placement, and final stats go here.")
