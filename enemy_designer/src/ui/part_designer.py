"""Part Designer tab UI. Placeholder layout for now."""

from __future__ import annotations

import dearpygui.dearpygui as dpg


def build_part_designer(*, parent: int | str) -> None:
    with dpg.group(parent=parent):
        dpg.add_text("Part Designer")
        dpg.add_separator()
        dpg.add_text("Placeholder — part list, image picker, and stats editing go here.")
