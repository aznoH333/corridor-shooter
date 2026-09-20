"""Enemy Designer tab UI."""

from __future__ import annotations

import math

import dearpygui.dearpygui as dpg

from src.formats import enemy_io, part_io
from src.models import Enemy, EnemyPartPlacement, MAX_ENEMY_PARTS, Part
from src.paths import ENEMIES_DIR, PARTS_DIR
from src.stats import fold_enemy
from src.ui import widgets

_ENEMIES: dict[str, Enemy] = {}
_PARTS: dict[str, Part] = {}
_SELECTED: str | None = None
_SAVED_NAME: str | None = None
_SELECTED_PART_IDX: int | None = None
_DRAGGING_IDX: int | None = None
_DRAG_GRAB: tuple[float, float] = (0.0, 0.0)
_SUPPRESS_EDIT = False

_TAG_LIST = "enemy_list"
_TAG_FORM = "enemy_form"
_TAG_EMPTY = "enemy_empty_hint"
_TAG_NAME = "enemy_name"
_TAG_STATUS = "enemy_status"
_TAG_FINAL_HEALTH = "enemy_final_health"
_TAG_FINAL_SPEED = "enemy_final_speed"
_TAG_FINAL_ACTION = "enemy_final_action"
_TAG_PART_LIST = "enemy_part_list"
_TAG_PART_FORM = "enemy_part_form"
_TAG_PART_DEF = "enemy_part_def"
_TAG_PART_X = "enemy_part_x"
_TAG_PART_Y = "enemy_part_y"
_TAG_PART_Z = "enemy_part_z"
_TAG_PART_ROT = "enemy_part_rot"
_TAG_CANVAS = "enemy_canvas"
_TAG_CANVAS_HINT = "enemy_canvas_hint"
_TAG_CHECKER_TEX = "enemy_checker_texture"
_TAG_TEX_REGISTRY = "enemy_texture_registry"
_TAG_ADD_PART_WINDOW = "enemy_add_part_window"
_TAG_ADD_PART_LIST = "enemy_add_part_list"
_TAG_HANDLERS = "enemy_canvas_handlers"

_CANVAS_ZOOM = 4
_CANVAS_LOGICAL = 160  # game pixels; display = logical * zoom
_CANVAS_SIZE = _CANVAS_LOGICAL * _CANVAS_ZOOM


def build_enemy_designer(*, parent: int | str) -> None:
    global _SELECTED, _SAVED_NAME, _SELECTED_PART_IDX

    ENEMIES_DIR.mkdir(parents=True, exist_ok=True)
    PARTS_DIR.mkdir(parents=True, exist_ok=True)
    _reload_libraries()
    _SELECTED = None
    _SAVED_NAME = None
    _SELECTED_PART_IDX = None

    with dpg.texture_registry(tag=_TAG_TEX_REGISTRY):
        widgets.ensure_placeholder_texture(
            texture_tag="enemy_placeholder_tex",
            parent=_TAG_TEX_REGISTRY,
        )
        widgets.ensure_checker_texture(
            texture_tag=_TAG_CHECKER_TEX,
            logical_w=_CANVAS_LOGICAL,
            logical_h=_CANVAS_LOGICAL,
            zoom=_CANVAS_ZOOM,
            parent=_TAG_TEX_REGISTRY,
        )

    with dpg.group(parent=parent, horizontal=True):
        # Left: enemy list
        with dpg.child_window(width=200, border=True):
            with dpg.group(horizontal=True):
                dpg.add_text("Enemies")
                dpg.add_button(label="+", width=30, callback=_on_add_enemy)
            dpg.add_separator()
            dpg.add_listbox(
                items=_sorted_enemy_names(),
                tag=_TAG_LIST,
                num_items=24,
                width=-1,
                callback=_on_enemy_list_selected,
            )

        # Middle: overview + part inspector
        with dpg.child_window(width=320, border=True):
            dpg.add_text(
                "Select an enemy or click + to create one.",
                tag=_TAG_EMPTY,
            )
            with dpg.group(tag=_TAG_FORM, show=False):
                dpg.add_text("Enemy")
                dpg.add_separator()
                dpg.add_input_text(label="Name", tag=_TAG_NAME, width=260)
                dpg.add_spacer(height=4)
                dpg.add_text("Final stats")
                dpg.add_text("health: —", tag=_TAG_FINAL_HEALTH)
                dpg.add_text("speed: —", tag=_TAG_FINAL_SPEED)
                dpg.add_text("action: —", tag=_TAG_FINAL_ACTION)
                dpg.add_separator()
                dpg.add_text(f"Parts (max {MAX_ENEMY_PARTS})")
                with dpg.group(horizontal=True):
                    dpg.add_button(label="Add part", callback=_on_add_part_clicked)
                    dpg.add_button(label="Remove part", callback=_on_remove_part)
                dpg.add_listbox(
                    items=[],
                    tag=_TAG_PART_LIST,
                    num_items=8,
                    width=-1,
                    callback=_on_part_list_selected,
                )
                with dpg.group(tag=_TAG_PART_FORM, show=False):
                    dpg.add_separator()
                    dpg.add_text("Selected part")
                    dpg.add_combo(
                        items=[],
                        tag=_TAG_PART_DEF,
                        label="Definition",
                        width=200,
                        callback=_on_part_def_changed,
                    )
                    dpg.add_input_float(
                        label="X",
                        tag=_TAG_PART_X,
                        width=140,
                        format="%.3g",
                        callback=_on_placement_edited,
                    )
                    dpg.add_input_float(
                        label="Y",
                        tag=_TAG_PART_Y,
                        width=140,
                        format="%.3g",
                        callback=_on_placement_edited,
                    )
                    dpg.add_input_float(
                        label="Z",
                        tag=_TAG_PART_Z,
                        width=140,
                        format="%.3g",
                        callback=_on_placement_edited,
                    )
                    dpg.add_input_float(
                        label="Rotation °",
                        tag=_TAG_PART_ROT,
                        width=140,
                        format="%.3g",
                        callback=_on_placement_edited,
                    )
                dpg.add_spacer(height=8)
                with dpg.group(horizontal=True):
                    dpg.add_button(label="Save", width=100, callback=_on_save)
                    dpg.add_button(label="Delete", width=100, callback=_on_delete)
            dpg.add_spacer(height=8)
            dpg.add_text("", tag=_TAG_STATUS, wrap=280)

        # Right: canvas
        with dpg.child_window(border=True):
            dpg.add_text("Canvas")
            dpg.add_text("Drag parts to move. Origin is the center.", tag=_TAG_CANVAS_HINT)
            dpg.add_separator()
            dpg.add_drawlist(
                width=_CANVAS_SIZE,
                height=_CANVAS_SIZE,
                tag=_TAG_CANVAS,
            )

    _build_add_part_window()
    _bind_canvas_handlers()
    _redraw_canvas()
    _show_empty_state("No enemies yet. Click + to create one.")


def _reload_libraries() -> None:
    _ENEMIES.clear()
    _ENEMIES.update(enemy_io.load_all_enemies(ENEMIES_DIR))
    _PARTS.clear()
    _PARTS.update(part_io.load_all_parts(PARTS_DIR))


def _sorted_enemy_names() -> list[str]:
    return sorted(_ENEMIES.keys())


def _set_status(message: str) -> None:
    if dpg.does_item_exist(_TAG_STATUS):
        dpg.set_value(_TAG_STATUS, message)


def _show_form(visible: bool) -> None:
    dpg.configure_item(_TAG_FORM, show=visible)
    dpg.configure_item(_TAG_EMPTY, show=not visible)


def _show_empty_state(message: str) -> None:
    global _SELECTED, _SAVED_NAME, _SELECTED_PART_IDX
    _SELECTED = None
    _SAVED_NAME = None
    _SELECTED_PART_IDX = None
    _show_form(False)
    dpg.set_value(_TAG_EMPTY, message)
    _set_status("")
    _redraw_canvas()


def _current_enemy() -> Enemy | None:
    if _SELECTED is None:
        return None
    return _ENEMIES.get(_SELECTED)


def _current_placement() -> EnemyPartPlacement | None:
    enemy = _current_enemy()
    if enemy is None or _SELECTED_PART_IDX is None:
        return None
    if not (0 <= _SELECTED_PART_IDX < len(enemy.parts)):
        return None
    return enemy.parts[_SELECTED_PART_IDX]


def _unique_enemy_name(base: str = "enemy") -> str:
    if base not in _ENEMIES:
        return base
    index = 2
    while f"{base}_{index}" in _ENEMIES:
        index += 1
    return f"{base}_{index}"


def _part_list_labels(enemy: Enemy) -> list[str]:
    labels = []
    for i, placement in enumerate(enemy.parts):
        labels.append(f"{i}: {placement.partName}")
    return labels


def _refresh_enemy_list(*, select: str | None = None) -> None:
    names = _sorted_enemy_names()
    dpg.configure_item(_TAG_LIST, items=names)
    if select and select in _ENEMIES:
        dpg.set_value(_TAG_LIST, select)
        _select_enemy(select)
    elif names:
        current = dpg.get_value(_TAG_LIST)
        if current in _ENEMIES:
            _select_enemy(current)
        else:
            dpg.set_value(_TAG_LIST, names[0])
            _select_enemy(names[0])
    else:
        _show_empty_state("No enemies yet. Click + to create one.")


def _on_enemy_list_selected(_sender, app_data, _user_data) -> None:
    if app_data:
        _select_enemy(app_data)


def _on_add_enemy() -> None:
    name = _unique_enemy_name()
    enemy = Enemy(name=name, parts=[])
    _ENEMIES[name] = enemy
    try:
        enemy_io.save_enemy(enemy, enemy_io.enemy_path(ENEMIES_DIR, name))
    except OSError as exc:
        del _ENEMIES[name]
        _set_status(f"Failed to create: {exc}")
        return
    _refresh_enemy_list(select=name)
    _set_status(f"Created {name}.enemy")


def _select_enemy(name: str) -> None:
    global _SELECTED, _SAVED_NAME, _SELECTED_PART_IDX, _SUPPRESS_EDIT
    enemy = _ENEMIES.get(name)
    if enemy is None:
        return
    _PARTS.clear()
    _PARTS.update(part_io.load_all_parts(PARTS_DIR))
    _SELECTED = name
    _SAVED_NAME = name
    _SELECTED_PART_IDX = 0 if enemy.parts else None
    _show_form(True)
    _SUPPRESS_EDIT = True
    dpg.set_value(_TAG_NAME, enemy.name)
    _SUPPRESS_EDIT = False
    _refresh_part_ui()
    _update_final_stats()
    _redraw_canvas()
    _set_status("")


def _refresh_part_ui() -> None:
    global _SUPPRESS_EDIT
    enemy = _current_enemy()
    if enemy is None:
        return

    labels = _part_list_labels(enemy)
    dpg.configure_item(_TAG_PART_LIST, items=labels)
    part_names = sorted(_PARTS.keys())
    dpg.configure_item(_TAG_PART_DEF, items=part_names)

    if _SELECTED_PART_IDX is not None and 0 <= _SELECTED_PART_IDX < len(enemy.parts):
        dpg.configure_item(_TAG_PART_FORM, show=True)
        if labels:
            dpg.set_value(_TAG_PART_LIST, labels[_SELECTED_PART_IDX])
        placement = enemy.parts[_SELECTED_PART_IDX]
        _SUPPRESS_EDIT = True
        dpg.set_value(_TAG_PART_DEF, placement.partName)
        dpg.set_value(_TAG_PART_X, placement.offsetX)
        dpg.set_value(_TAG_PART_Y, placement.offsetY)
        dpg.set_value(_TAG_PART_Z, placement.offsetZ)
        dpg.set_value(_TAG_PART_ROT, math.degrees(placement.rotation))
        _SUPPRESS_EDIT = False
    else:
        dpg.configure_item(_TAG_PART_FORM, show=False)


def _update_final_stats() -> None:
    enemy = _current_enemy()
    if enemy is None:
        return
    stats = fold_enemy(enemy, _PARTS)
    dpg.set_value(_TAG_FINAL_HEALTH, f"health: {stats.health:g}")
    dpg.set_value(_TAG_FINAL_SPEED, f"speed: {stats.speed:g}")
    dpg.set_value(_TAG_FINAL_ACTION, f"action: {stats.action:g}")


def _on_part_list_selected(_sender, app_data, _user_data) -> None:
    global _SELECTED_PART_IDX
    enemy = _current_enemy()
    if enemy is None or not app_data:
        return
    try:
        _SELECTED_PART_IDX = int(str(app_data).split(":", 1)[0])
    except ValueError:
        return
    _refresh_part_ui()
    _redraw_canvas()


def _on_part_def_changed(_sender, app_data, _user_data) -> None:
    if _SUPPRESS_EDIT:
        return
    placement = _current_placement()
    if placement is None or not app_data:
        return
    if app_data not in _PARTS:
        _set_status(f"Unknown part '{app_data}'")
        return
    placement.partName = app_data
    _refresh_part_ui()
    _update_final_stats()
    _redraw_canvas()


def _on_placement_edited(_sender, _app_data, _user_data) -> None:
    if _SUPPRESS_EDIT:
        return
    placement = _current_placement()
    if placement is None:
        return
    placement.offsetX = float(dpg.get_value(_TAG_PART_X))
    placement.offsetY = float(dpg.get_value(_TAG_PART_Y))
    placement.offsetZ = float(dpg.get_value(_TAG_PART_Z))
    placement.rotation = math.radians(float(dpg.get_value(_TAG_PART_ROT)))
    _redraw_canvas()


def _build_add_part_window() -> None:
    if dpg.does_item_exist(_TAG_ADD_PART_WINDOW):
        return
    with dpg.window(
        label="Add part",
        modal=True,
        show=False,
        tag=_TAG_ADD_PART_WINDOW,
        width=320,
        height=360,
        no_collapse=True,
    ):
        dpg.add_text("Choose a part definition")
        dpg.add_listbox(items=[], tag=_TAG_ADD_PART_LIST, num_items=12, width=-1)
        with dpg.group(horizontal=True):
            dpg.add_button(label="Add", callback=_on_confirm_add_part, width=100)
            dpg.add_button(
                label="Cancel",
                callback=lambda: dpg.configure_item(_TAG_ADD_PART_WINDOW, show=False),
                width=100,
            )


def _on_add_part_clicked() -> None:
    enemy = _current_enemy()
    if enemy is None:
        return
    if len(enemy.parts) >= MAX_ENEMY_PARTS:
        _set_status(f"Max {MAX_ENEMY_PARTS} parts reached.")
        return
    _PARTS.clear()
    _PARTS.update(part_io.load_all_parts(PARTS_DIR))
    names = sorted(_PARTS.keys())
    if not names:
        _set_status("No parts available. Create one in Part Designer first.")
        return
    dpg.configure_item(_TAG_ADD_PART_LIST, items=names)
    dpg.set_value(_TAG_ADD_PART_LIST, names[0])
    dpg.configure_item(_TAG_ADD_PART_WINDOW, show=True)


def _on_confirm_add_part() -> None:
    global _SELECTED_PART_IDX
    enemy = _current_enemy()
    if enemy is None:
        return
    if len(enemy.parts) >= MAX_ENEMY_PARTS:
        _set_status(f"Max {MAX_ENEMY_PARTS} parts reached.")
        dpg.configure_item(_TAG_ADD_PART_WINDOW, show=False)
        return
    name = dpg.get_value(_TAG_ADD_PART_LIST)
    if not name or name not in _PARTS:
        _set_status("Select a valid part.")
        return
    enemy.parts.append(
        EnemyPartPlacement(partName=name, offsetX=0, offsetY=0, offsetZ=0, rotation=0)
    )
    _SELECTED_PART_IDX = len(enemy.parts) - 1
    dpg.configure_item(_TAG_ADD_PART_WINDOW, show=False)
    _refresh_part_ui()
    _update_final_stats()
    _redraw_canvas()
    _set_status(f"Added part '{name}'")


def _on_remove_part() -> None:
    global _SELECTED_PART_IDX
    enemy = _current_enemy()
    if enemy is None or _SELECTED_PART_IDX is None:
        _set_status("No part selected.")
        return
    if not (0 <= _SELECTED_PART_IDX < len(enemy.parts)):
        return
    removed = enemy.parts.pop(_SELECTED_PART_IDX)
    if enemy.parts:
        _SELECTED_PART_IDX = min(_SELECTED_PART_IDX, len(enemy.parts) - 1)
    else:
        _SELECTED_PART_IDX = None
    _refresh_part_ui()
    _update_final_stats()
    _redraw_canvas()
    _set_status(f"Removed part '{removed.partName}'")


def _on_save() -> None:
    global _SELECTED, _SAVED_NAME
    enemy = _current_enemy()
    if enemy is None:
        _set_status("Nothing to save.")
        return

    name = dpg.get_value(_TAG_NAME).strip()
    if not name:
        _set_status("Name cannot be empty.")
        return
    if name != enemy.name and name in _ENEMIES:
        _set_status(f"An enemy named '{name}' already exists.")
        return

    old_name = _SAVED_NAME
    enemy.name = name

    if old_name and old_name != name:
        del _ENEMIES[old_name]
        _ENEMIES[name] = enemy
        old_path = enemy_io.enemy_path(ENEMIES_DIR, old_name)
        try:
            enemy_io.save_enemy(enemy, enemy_io.enemy_path(ENEMIES_DIR, name))
            if old_path.is_file():
                old_path.unlink()
        except OSError as exc:
            del _ENEMIES[name]
            _ENEMIES[old_name] = enemy
            enemy.name = old_name
            _set_status(f"Failed to save: {exc}")
            return
    else:
        _ENEMIES[name] = enemy
        try:
            enemy_io.save_enemy(enemy, enemy_io.enemy_path(ENEMIES_DIR, name))
        except OSError as exc:
            _set_status(f"Failed to save: {exc}")
            return

    _SELECTED = name
    _SAVED_NAME = name
    _refresh_enemy_list(select=name)
    _set_status(f"Saved {name}.enemy")


def _on_delete() -> None:
    global _SELECTED, _SAVED_NAME, _SELECTED_PART_IDX
    enemy = _current_enemy()
    if enemy is None:
        _set_status("Nothing to delete.")
        return
    name = _SAVED_NAME or enemy.name
    path = enemy_io.enemy_path(ENEMIES_DIR, name)
    try:
        if path.is_file():
            path.unlink()
    except OSError as exc:
        _set_status(f"Failed to delete: {exc}")
        return
    _ENEMIES.pop(name, None)
    _ENEMIES.pop(enemy.name, None)
    _SELECTED = None
    _SAVED_NAME = None
    _SELECTED_PART_IDX = None
    _refresh_enemy_list()
    _set_status(f"Deleted {name}.enemy")


# ---------------------------------------------------------------------------
# Canvas
# ---------------------------------------------------------------------------

def _bind_canvas_handlers() -> None:
    if dpg.does_item_exist(_TAG_HANDLERS):
        return
    with dpg.handler_registry(tag=_TAG_HANDLERS):
        dpg.add_mouse_click_handler(button=0, callback=_on_canvas_click)
        dpg.add_mouse_drag_handler(button=0, threshold=1, callback=_on_canvas_drag)
        dpg.add_mouse_release_handler(button=0, callback=_on_canvas_release)


def _canvas_mouse_logical() -> tuple[float, float] | None:
    if not dpg.does_item_exist(_TAG_CANVAS) or not dpg.is_item_hovered(_TAG_CANVAS):
        return None
    mouse = dpg.get_mouse_pos(local=False)
    rect_min = dpg.get_item_rect_min(_TAG_CANVAS)
    local_x = mouse[0] - rect_min[0]
    local_y = mouse[1] - rect_min[1]
    origin = _CANVAS_SIZE / 2.0
    logical_x = (local_x - origin) / _CANVAS_ZOOM
    logical_y = (local_y - origin) / _CANVAS_ZOOM
    return logical_x, logical_y


def _placement_source_size(placement: EnemyPartPlacement) -> tuple[int, int]:
    part = _PARTS.get(placement.partName)
    if part is None:
        return 16, 16
    return max(1, part.textureSizeX), max(1, part.textureSizeY)


def _hit_test_part(placement: EnemyPartPlacement, lx: float, ly: float) -> bool:
    """Axis-aligned hit test in logical pixels (ignores rotation for grab ease)."""
    sw, sh = _placement_source_size(placement)
    hw, hh = sw / 2.0, sh / 2.0
    dx = lx - placement.offsetX
    dy = ly - placement.offsetY
    return abs(dx) <= hw and abs(dy) <= hh


def _on_canvas_click(_sender, _app_data, _user_data) -> None:
    global _DRAGGING_IDX, _DRAG_GRAB, _SELECTED_PART_IDX
    enemy = _current_enemy()
    if enemy is None:
        return
    mouse = _canvas_mouse_logical()
    if mouse is None:
        return
    lx, ly = mouse

    # Top-most first (highest z, then later list index).
    order = sorted(
        range(len(enemy.parts)),
        key=lambda i: (enemy.parts[i].offsetZ, i),
        reverse=True,
    )
    for index in order:
        placement = enemy.parts[index]
        if _hit_test_part(placement, lx, ly):
            _SELECTED_PART_IDX = index
            _DRAGGING_IDX = index
            _DRAG_GRAB = (lx - placement.offsetX, ly - placement.offsetY)
            _refresh_part_ui()
            _redraw_canvas()
            return
    _DRAGGING_IDX = None


def _on_canvas_drag(_sender, _app_data, _user_data) -> None:
    global _SUPPRESS_EDIT
    enemy = _current_enemy()
    if enemy is None or _DRAGGING_IDX is None:
        return
    if not (0 <= _DRAGGING_IDX < len(enemy.parts)):
        return
    mouse = _canvas_mouse_logical()
    if mouse is None:
        return
    lx, ly = mouse
    placement = enemy.parts[_DRAGGING_IDX]
    placement.offsetX = round(lx - _DRAG_GRAB[0])
    placement.offsetY = round(ly - _DRAG_GRAB[1])
    _SUPPRESS_EDIT = True
    dpg.set_value(_TAG_PART_X, placement.offsetX)
    dpg.set_value(_TAG_PART_Y, placement.offsetY)
    _SUPPRESS_EDIT = False
    _redraw_canvas()


def _on_canvas_release(_sender, _app_data, _user_data) -> None:
    global _DRAGGING_IDX
    _DRAGGING_IDX = None


def _rotated_quad(
    cx: float, cy: float, width: float, height: float, angle: float
) -> list[tuple[float, float]]:
    hw, hh = width / 2.0, height / 2.0
    corners = ((-hw, -hh), (hw, -hh), (hw, hh), (-hw, hh))
    cos_a = math.cos(angle)
    sin_a = math.sin(angle)
    result: list[tuple[float, float]] = []
    for x, y in corners:
        result.append((cx + x * cos_a - y * sin_a, cy + x * sin_a + y * cos_a))
    return result


def _ensure_part_sprite(part: Part) -> tuple[str, int, int] | None:
    path = widgets.resolve_texture_path(part.texture)
    if path is None:
        return None
    tag = f"enemy_sprite_{part.texture}"
    size = widgets.ensure_sprite_texture(
        path,
        texture_tag=tag,
        zoom=_CANVAS_ZOOM,
        parent=_TAG_TEX_REGISTRY,
    )
    if size is None:
        return None
    dw, dh, _sw, _sh = size
    return tag, dw, dh


def _redraw_canvas() -> None:
    if not dpg.does_item_exist(_TAG_CANVAS):
        return
    dpg.delete_item(_TAG_CANVAS, children_only=True)

    # Checkerboard background (1 logical px per cell, already zoomed in texture).
    dpg.draw_image(
        _TAG_CHECKER_TEX,
        (0, 0),
        (_CANVAS_SIZE, _CANVAS_SIZE),
        parent=_TAG_CANVAS,
    )

    origin = _CANVAS_SIZE / 2.0
    # Origin crosshair
    dpg.draw_line(
        (origin - 12, origin),
        (origin + 12, origin),
        color=(255, 255, 255, 120),
        thickness=1,
        parent=_TAG_CANVAS,
    )
    dpg.draw_line(
        (origin, origin - 12),
        (origin, origin + 12),
        color=(255, 255, 255, 120),
        thickness=1,
        parent=_TAG_CANVAS,
    )

    enemy = _current_enemy()
    if enemy is None:
        return

    draw_order = sorted(
        range(len(enemy.parts)),
        key=lambda i: (enemy.parts[i].offsetZ, i),
    )
    for index in draw_order:
        placement = enemy.parts[index]
        part = _PARTS.get(placement.partName)
        if part is None:
            continue
        sprite = _ensure_part_sprite(part)
        if sprite is None:
            continue
        tex_tag, dw, dh = sprite
        cx = origin + placement.offsetX * _CANVAS_ZOOM
        cy = origin + placement.offsetY * _CANVAS_ZOOM
        p1, p2, p3, p4 = _rotated_quad(cx, cy, dw, dh, placement.rotation)
        dpg.draw_image_quad(
            tex_tag,
            p1,
            p2,
            p3,
            p4,
            parent=_TAG_CANVAS,
        )
        if index == _SELECTED_PART_IDX:
            # Selection outline (unrotated AABB in display space).
            dpg.draw_rectangle(
                (cx - dw / 2 - 2, cy - dh / 2 - 2),
                (cx + dw / 2 + 2, cy + dh / 2 + 2),
                color=(255, 220, 80, 220),
                thickness=2,
                parent=_TAG_CANVAS,
            )
