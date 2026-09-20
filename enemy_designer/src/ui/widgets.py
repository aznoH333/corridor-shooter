"""Shared Dear PyGui widgets and texture helpers."""

from __future__ import annotations

from pathlib import Path

import dearpygui.dearpygui as dpg
from PIL import Image

from src.paths import TEXTURES_DIR


def texture_id_from_path(path: Path | str) -> str:
    """Game texture id is the PNG filename stem (no extension)."""
    return Path(path).stem


def resolve_texture_path(texture: str) -> Path | None:
    """Resolve a game texture id (or path) to a PNG on disk."""
    if not texture:
        return None
    candidate = Path(texture)
    if candidate.is_file():
        return candidate
    png = TEXTURES_DIR / f"{texture}.png"
    if png.is_file():
        return png
    return None


def png_size(path: Path | str) -> tuple[int, int]:
    with Image.open(path) as image:
        return image.size


def ensure_png_file_dialog(
    *,
    tag: str,
    callback,
    default_path: str | Path = TEXTURES_DIR,
    width: int = 780,
    height: int = 520,
) -> None:
    """Create a hidden PNG file dialog once; call dpg.show_item(tag) to open."""
    if dpg.does_item_exist(tag):
        return

    path = str(Path(default_path))
    with dpg.file_dialog(
        tag=tag,
        directory_selector=False,
        show=False,
        modal=True,
        callback=callback,
        default_path=path,
        width=width,
        height=height,
    ):
        dpg.add_file_extension("PNG files (.png){.png}", color=(0, 255, 150, 255))
        dpg.add_file_extension(".*")


def selected_path_from_dialog(app_data) -> Path | None:
    """Extract the first chosen file path from a Dear PyGui file-dialog callback."""
    paths = selected_paths_from_dialog(app_data)
    return paths[0] if paths else None


def selected_paths_from_dialog(app_data) -> list[Path]:
    """Extract all chosen file paths from a Dear PyGui file-dialog callback."""
    if not app_data:
        return []
    selections = app_data.get("selections") or {}
    if selections:
        return [Path(path) for path in selections.values()]
    file_path_name = app_data.get("file_path_name")
    if file_path_name:
        return [Path(file_path_name)]
    return []


def load_preview_texture(
    path: Path,
    *,
    texture_tag: str,
    parent: int | str = 0,
    max_side: int = 520,
) -> tuple[int, int, int, int] | None:
    """Load a pixel-art PNG with a 1px green checkerboard, nearest-neighbor upscaled.

    Returns (display_w, display_h, source_w, source_h).
    Display size is an integer multiple of the source so DPG shows 1:1 texels (no filtering).
    """
    if not path.is_file():
        return None

    rgba, display_w, display_h, source_w, source_h = _pixel_art_preview_rgba(
        path, max_side=max_side
    )
    kwargs = {"tag": texture_tag}
    if parent:
        kwargs["parent"] = parent
    dpg.add_static_texture(display_w, display_h, rgba, **kwargs)
    return display_w, display_h, source_w, source_h


def ensure_placeholder_texture(*, texture_tag: str, parent: int | str = 0) -> None:
    """Create a 1x1 transparent texture so image widgets can bind before first load."""
    if dpg.does_item_exist(texture_tag):
        return
    kwargs = {"tag": texture_tag}
    if parent:
        kwargs["parent"] = parent
    dpg.add_static_texture(1, 1, [0.0, 0.0, 0.0, 0.0], **kwargs)


# Checkerboard colors for transparent pixel-art preview (#004c33 / #003235).
CHECKER_A = (0x00, 0x4C, 0x33, 255)
CHECKER_B = (0x00, 0x32, 0x35, 255)


def _pixel_art_preview_rgba(
    path: Path, *, max_side: int
) -> tuple[list[float], int, int, int, int]:
    with Image.open(path) as image:
        sprite = image.convert("RGBA")

    source_w, source_h = sprite.size
    checker = make_checkerboard(source_w, source_h)
    composited = Image.alpha_composite(checker, sprite)

    scale = max(1, min(max_side // source_w, max_side // source_h, 8))
    display_w = source_w * scale
    display_h = source_h * scale
    if scale != 1:
        composited = composited.resize((display_w, display_h), Image.Resampling.NEAREST)

    return _image_to_float_rgba(composited), display_w, display_h, source_w, source_h


def make_checkerboard(width: int, height: int) -> Image.Image:
    """1 source-pixel per checker cell."""
    pixels = [
        CHECKER_A if ((x + y) & 1) == 0 else CHECKER_B
        for y in range(height)
        for x in range(width)
    ]
    image = Image.new("RGBA", (width, height))
    image.putdata(pixels)
    return image


def ensure_checker_texture(
    *,
    texture_tag: str,
    logical_w: int,
    logical_h: int,
    zoom: int,
    parent: int | str = 0,
) -> tuple[int, int]:
    """Upload a nearest-neighbor-scaled checkerboard texture. Returns display size."""
    if dpg.does_item_exist(texture_tag):
        dpg.delete_item(texture_tag)

    checker = make_checkerboard(logical_w, logical_h)
    display_w = logical_w * zoom
    display_h = logical_h * zoom
    if zoom != 1:
        checker = checker.resize((display_w, display_h), Image.Resampling.NEAREST)

    kwargs = {"tag": texture_tag}
    if parent:
        kwargs["parent"] = parent
    dpg.add_static_texture(display_w, display_h, _image_to_float_rgba(checker), **kwargs)
    return display_w, display_h


def ensure_sprite_texture(
    path: Path,
    *,
    texture_tag: str,
    zoom: int,
    parent: int | str = 0,
) -> tuple[int, int, int, int] | None:
    """Load a sprite nearest-neighbor upscaled by zoom. Returns (dw, dh, sw, sh)."""
    if not path.is_file():
        return None

    with Image.open(path) as image:
        sprite = image.convert("RGBA")
    source_w, source_h = sprite.size
    display_w = source_w * zoom
    display_h = source_h * zoom
    if zoom != 1:
        sprite = sprite.resize((display_w, display_h), Image.Resampling.NEAREST)

    if not dpg.does_item_exist(texture_tag):
        kwargs = {"tag": texture_tag}
        if parent:
            kwargs["parent"] = parent
        dpg.add_static_texture(
            display_w, display_h, _image_to_float_rgba(sprite), **kwargs
        )
    return display_w, display_h, source_w, source_h


def _image_to_float_rgba(image: Image.Image) -> list[float]:
    rgba: list[float] = []
    for r, g, b, a in image.getdata():
        rgba.extend((r / 255.0, g / 255.0, b / 255.0, a / 255.0))
    return rgba
