"""Part Designer tab UI."""

from __future__ import annotations

import dearpygui.dearpygui as dpg

from src.formats import part_io
from src.models import Part, default_part_stats
from src.paths import PARTS_DIR, TEXTURES_DIR
from src.ui import widgets

_PARTS: dict[str, Part] = {}
_SELECTED: str | None = None
_SAVED_NAME: str | None = None  # last persisted filename stem for rename/delete
_PICK_MODE: str = "create"  # "create" | "change_texture"

_TAG_LIST = "part_list"
_TAG_NAME = "part_name"
_TAG_TEXTURE = "part_texture"
_TAG_SIZE = "part_size"
_TAG_STATUS = "part_status"
_TAG_PREVIEW = "part_preview_image"
_TAG_PREVIEW_TEX = "part_preview_texture"
_TAG_DIALOG = "part_texture_dialog"
_TAG_FORM = "part_form"
_TAG_EMPTY = "part_empty_hint"
_TAG_PREVIEW_SIZE = "part_preview_size_label"
_PREVIEW_TEX_COUNTER = 0
_ACTIVE_PREVIEW_TEX = _TAG_PREVIEW_TEX
_PREVIEW_MAX_SIDE = 520

_STAT_FIELDS = (
    ("health", "part_stat_health"),
    ("healthMult", "part_stat_health_mult"),
    ("speed", "part_stat_speed"),
    ("speedMult", "part_stat_speed_mult"),
    ("action", "part_stat_action"),
    ("actionMult", "part_stat_action_mult"),
)


def build_part_designer(*, parent: int | str) -> None:
    global _ACTIVE_PREVIEW_TEX, _PREVIEW_TEX_COUNTER, _SELECTED, _SAVED_NAME

    PARTS_DIR.mkdir(parents=True, exist_ok=True)
    _PARTS.clear()
    _PARTS.update(part_io.load_all_parts(PARTS_DIR))
    _SELECTED = None
    _SAVED_NAME = None
    _PREVIEW_TEX_COUNTER = 0
    _ACTIVE_PREVIEW_TEX = _TAG_PREVIEW_TEX

    with dpg.texture_registry(tag="part_texture_registry"):
        widgets.ensure_placeholder_texture(
            texture_tag=_TAG_PREVIEW_TEX,
            parent="part_texture_registry",
        )
    widgets.ensure_png_file_dialog(
        tag=_TAG_DIALOG,
        callback=_on_texture_picked,
        default_path=TEXTURES_DIR,
    )

    with dpg.group(parent=parent, horizontal=True):
        # Left: part list
        with dpg.child_window(width=220, border=True):
            with dpg.group(horizontal=True):
                dpg.add_text("Parts")
                dpg.add_button(label="+", width=30, callback=_on_add_clicked)
            dpg.add_separator()
            dpg.add_listbox(
                items=_sorted_names(),
                tag=_TAG_LIST,
                num_items=24,
                width=-1,
                callback=_on_list_selected,
            )

        # Middle: inputs (hidden until a part is selected)
        with dpg.child_window(width=340, border=True):
            dpg.add_text(
                "Select a part or click + to create one.",
                tag=_TAG_EMPTY,
            )
            with dpg.group(tag=_TAG_FORM, show=False):
                dpg.add_text("Part")
                dpg.add_separator()
                dpg.add_input_text(label="Name", tag=_TAG_NAME, width=280)
                with dpg.group(horizontal=True):
                    dpg.add_input_text(
                        label="Texture",
                        tag=_TAG_TEXTURE,
                        width=200,
                        readonly=True,
                    )
                    dpg.add_button(label="Change…", callback=_on_change_texture_clicked)
                dpg.add_text("Size: —", tag=_TAG_SIZE)
                dpg.add_separator()
                dpg.add_text("Stats")
                for label, tag in _STAT_FIELDS:
                    dpg.add_input_float(label=label, tag=tag, width=160, format="%.4g")
                dpg.add_spacer(height=8)
                with dpg.group(horizontal=True):
                    dpg.add_button(label="Save", callback=_on_save_clicked, width=100)
                    dpg.add_button(label="Delete", callback=_on_delete_clicked, width=100)
            dpg.add_spacer(height=8)
            dpg.add_text("", tag=_TAG_STATUS, wrap=300)

        # Right: large preview
        with dpg.child_window(border=True):
            dpg.add_text("Preview")
            dpg.add_text("", tag=_TAG_PREVIEW_SIZE)
            dpg.add_separator()
            dpg.add_image(_TAG_PREVIEW_TEX, tag=_TAG_PREVIEW, show=False)

    _show_empty_state("No parts yet. Click + to create one from a PNG.")


def _sorted_names() -> list[str]:
    return sorted(_PARTS.keys())


def _set_status(message: str) -> None:
    if dpg.does_item_exist(_TAG_STATUS):
        dpg.set_value(_TAG_STATUS, message)


def _show_form(visible: bool) -> None:
    dpg.configure_item(_TAG_FORM, show=visible)
    dpg.configure_item(_TAG_EMPTY, show=not visible)


def _show_empty_state(message: str = "Select a part or click + to create one.") -> None:
    _clear_selection_state()
    _show_form(False)
    if dpg.does_item_exist(_TAG_PREVIEW):
        dpg.configure_item(_TAG_PREVIEW, show=False)
    if dpg.does_item_exist(_TAG_PREVIEW_SIZE):
        dpg.set_value(_TAG_PREVIEW_SIZE, "")
    dpg.set_value(_TAG_EMPTY, message)
    _set_status("")


def _clear_selection_state() -> None:
    global _SELECTED, _SAVED_NAME
    _SELECTED = None
    _SAVED_NAME = None


def _refresh_list(*, select: str | None = None) -> None:
    names = _sorted_names()
    dpg.configure_item(_TAG_LIST, items=names)
    if select and select in _PARTS:
        dpg.set_value(_TAG_LIST, select)
        _select_part(select)
    elif names:
        current = dpg.get_value(_TAG_LIST)
        if current in _PARTS:
            _select_part(current)
        else:
            dpg.set_value(_TAG_LIST, names[0])
            _select_part(names[0])
    else:
        _show_empty_state("No parts yet. Click + to create one from a PNG.")


def _on_list_selected(_sender, app_data, _user_data) -> None:
    if app_data:
        _select_part(app_data)


def _on_add_clicked() -> None:
    global _PICK_MODE
    _PICK_MODE = "create"
    dpg.configure_item(_TAG_DIALOG, default_path=str(TEXTURES_DIR))
    dpg.show_item(_TAG_DIALOG)


def _on_change_texture_clicked() -> None:
    global _PICK_MODE
    if _SELECTED is None:
        _set_status("Select a part first.")
        return
    _PICK_MODE = "change_texture"
    dpg.configure_item(_TAG_DIALOG, default_path=str(TEXTURES_DIR))
    dpg.show_item(_TAG_DIALOG)


def _on_texture_picked(_sender, app_data, _user_data) -> None:
    path = widgets.selected_path_from_dialog(app_data)
    if path is None:
        return
    if path.suffix.lower() != ".png":
        _set_status(f"Expected a PNG, got {path.name}")
        return
    if not path.is_file():
        _set_status(f"File not found: {path}")
        return

    try:
        width, height = widgets.png_size(path)
    except OSError as exc:
        _set_status(f"Could not read PNG: {exc}")
        return

    texture_id = widgets.texture_id_from_path(path)

    if _PICK_MODE == "create":
        _create_part_from_image(texture_id, width, height)
    else:
        _apply_texture_to_selected(texture_id, width, height)


def _unique_name(base: str) -> str:
    if base not in _PARTS:
        return base
    index = 2
    while f"{base}_{index}" in _PARTS:
        index += 1
    return f"{base}_{index}"


def _create_part_from_image(texture_id: str, width: int, height: int) -> None:
    name = _unique_name(texture_id)
    part = Part(
        name=name,
        texture=texture_id,
        textureSizeX=width,
        textureSizeY=height,
        stats=default_part_stats(),
    )
    _PARTS[name] = part
    try:
        part_io.save_part(part, part_io.part_path(PARTS_DIR, name))
    except OSError as exc:
        del _PARTS[name]
        _set_status(f"Failed to save: {exc}")
        return

    global _SAVED_NAME
    _SAVED_NAME = name
    _refresh_list(select=name)
    _set_status(f"Created {name}.part")


def _apply_texture_to_selected(texture_id: str, width: int, height: int) -> None:
    part = _current_part()
    if part is None:
        return
    part.texture = texture_id
    part.textureSizeX = width
    part.textureSizeY = height
    dpg.set_value(_TAG_TEXTURE, texture_id)
    dpg.set_value(_TAG_SIZE, f"Size: {width} × {height}")
    _update_preview(part)
    _set_status(f"Texture set to {texture_id} ({width}×{height}). Save to persist.")


def _current_part() -> Part | None:
    if _SELECTED is None:
        return None
    return _PARTS.get(_SELECTED)


def _select_part(name: str) -> None:
    global _SELECTED, _SAVED_NAME
    part = _PARTS.get(name)
    if part is None:
        return
    _SELECTED = name
    _SAVED_NAME = name
    _show_form(True)
    dpg.set_value(_TAG_NAME, part.name)
    dpg.set_value(_TAG_TEXTURE, part.texture)
    dpg.set_value(_TAG_SIZE, f"Size: {part.textureSizeX} × {part.textureSizeY}")
    for attr, tag in _STAT_FIELDS:
        dpg.set_value(tag, getattr(part.stats, attr))
    _update_preview(part)
    _set_status("")


def _update_preview(part: Part) -> None:
    global _PREVIEW_TEX_COUNTER, _ACTIVE_PREVIEW_TEX

    path = widgets.resolve_texture_path(part.texture)
    if path is None:
        if dpg.does_item_exist(_TAG_PREVIEW):
            dpg.configure_item(_TAG_PREVIEW, show=False)
        dpg.set_value(_TAG_PREVIEW_SIZE, "Texture not found")
        return

    _PREVIEW_TEX_COUNTER += 1
    new_tag = f"{_TAG_PREVIEW_TEX}_{_PREVIEW_TEX_COUNTER}"
    size = widgets.load_preview_texture(
        path,
        texture_tag=new_tag,
        parent="part_texture_registry",
        max_side=_PREVIEW_MAX_SIDE,
    )
    if size is None:
        dpg.configure_item(_TAG_PREVIEW, show=False)
        dpg.set_value(_TAG_PREVIEW_SIZE, "Failed to load preview")
        return

    display_w, display_h, source_w, source_h = size
    old_tag = _ACTIVE_PREVIEW_TEX
    _ACTIVE_PREVIEW_TEX = new_tag
    # Texture is already nearest-neighbor upscaled; show 1:1 to avoid GPU filtering.
    dpg.configure_item(
        _TAG_PREVIEW,
        texture_tag=new_tag,
        width=display_w,
        height=display_h,
        show=True,
    )
    dpg.set_value(_TAG_PREVIEW_SIZE, f"{source_w} × {source_h} px")
    if old_tag != new_tag and dpg.does_item_exist(old_tag) and old_tag != _TAG_PREVIEW_TEX:
        try:
            dpg.delete_item(old_tag)
        except SystemError:
            pass


def _read_inspector_into(part: Part) -> str | None:
    """Apply inspector fields to part. Returns error message or None."""
    name = dpg.get_value(_TAG_NAME).strip()
    if not name:
        return "Name cannot be empty."
    if name != part.name and name in _PARTS:
        return f"A part named '{name}' already exists."

    part.name = name
    part.texture = dpg.get_value(_TAG_TEXTURE).strip()
    for attr, tag in _STAT_FIELDS:
        setattr(part.stats, attr, float(dpg.get_value(tag)))
    return None


def _on_save_clicked() -> None:
    global _SELECTED, _SAVED_NAME
    part = _current_part()
    if part is None:
        _set_status("Nothing to save.")
        return

    error = _read_inspector_into(part)
    if error:
        _set_status(error)
        return

    old_name = _SAVED_NAME
    new_name = part.name

    if old_name and old_name != new_name:
        del _PARTS[old_name]
        _PARTS[new_name] = part
        old_path = part_io.part_path(PARTS_DIR, old_name)
        try:
            part_io.save_part(part, part_io.part_path(PARTS_DIR, new_name))
            if old_path.is_file():
                old_path.unlink()
        except OSError as exc:
            del _PARTS[new_name]
            _PARTS[old_name] = part
            part.name = old_name
            _set_status(f"Failed to save: {exc}")
            return
    else:
        _PARTS[new_name] = part
        try:
            part_io.save_part(part, part_io.part_path(PARTS_DIR, new_name))
        except OSError as exc:
            _set_status(f"Failed to save: {exc}")
            return

    _SELECTED = new_name
    _SAVED_NAME = new_name
    _refresh_list(select=new_name)
    _set_status(f"Saved {new_name}.part")


def _on_delete_clicked() -> None:
    global _SELECTED, _SAVED_NAME
    part = _current_part()
    if part is None:
        _set_status("Nothing to delete.")
        return

    name = _SAVED_NAME or part.name
    path = part_io.part_path(PARTS_DIR, name)
    try:
        if path.is_file():
            path.unlink()
    except OSError as exc:
        _set_status(f"Failed to delete: {exc}")
        return

    _PARTS.pop(name, None)
    _PARTS.pop(part.name, None)
    _SELECTED = None
    _SAVED_NAME = None
    _refresh_list()
    _set_status(f"Deleted {name}.part")
