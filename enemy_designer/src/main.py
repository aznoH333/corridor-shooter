"""Enemy Designer entry point: window + Part / Enemy tabs."""

from __future__ import annotations

import dearpygui.dearpygui as dpg

from src.ui.enemy_designer import build_enemy_designer
from src.ui.part_designer import build_part_designer


def main() -> None:
    dpg.create_context()
    dpg.set_global_font_scale(1.25)
    dpg.create_viewport(title="Enemy Designer", width=1280, height=800)

    with dpg.window(tag="primary_window", label="Enemy Designer"):
        with dpg.tab_bar(tag="main_tabs"):
            with dpg.tab(label="Part Designer", tag="tab_part_designer"):
                build_part_designer(parent="tab_part_designer")
            with dpg.tab(label="Enemy Designer", tag="tab_enemy_designer"):
                build_enemy_designer(parent="tab_enemy_designer")

    dpg.setup_dearpygui()
    dpg.show_viewport()
    dpg.set_primary_window("primary_window", True)
    dpg.start_dearpygui()
    dpg.destroy_context()


if __name__ == "__main__":
    main()
