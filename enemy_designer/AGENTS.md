# Enemy Designer — Agent Guide

Internal desktop tool for authoring reusable enemy **parts** and composing them into **enemies** for corridor-shooter. Output is custom text formats consumed (or later imported) by the C game.

## Tech stack

Use **Python 3.11+** with **Dear PyGui**.

| Choice | Why |
| --- | --- |
| Python | Easy for agents to read/edit; fast iteration |
| Dear PyGui | Cross-platform (Linux + macOS), real desktop UI, good for image preview + 2D canvas, lighter than Electron |

Do **not** use Electron unless Dear PyGui is blocked. Prefer a single-window app with two modes/tabs: **Part Designer** and **Enemy Designer**.

### Layout (suggested)

```
enemy_designer/
  AGENTS.md
  README.md
  pyproject.toml / requirements.txt
  src/
    main.py              # entry, window, tabs
    models.py            # Part, Enemy, EnemyStats dataclasses
    formats/
      part_io.py         # .part read/write
      enemy_io.py        # .enemy read/write
    stats.py             # combineStats (must match C)
    ui/
      part_designer.py
      enemy_designer.py
      widgets.py         # image picker, numeric fields
  assets/                # optional sample textures for dev
  parts/                 # user .part files + referenced PNGs
  enemies/               # user .enemy files
```

Keep game-format logic in `formats/` and `stats.py` — no parsing inside UI widgets.

## Domain model (mirrors game C)

Canonical C lives in `../src/enemies.h` and `../src/enemies.c`. Keep tool field names aligned.

### EnemyStats

| Field | Meaning |
| --- | --- |
| `health`, `healthMult` | Hit points base / multiplier |
| `speed`, `speedMult` | Movement |
| `action`, `actionMult` | Action cooldown (lower = faster) |

### Part (`EnemyPartDefinition` + name)

| Field | Notes |
| --- | --- |
| `name` | Tool-only id; used by enemies to reference the part |
| `texture` | PNG path or texture id string (game currently uses names like `"picus"`) |
| `textureSizeX`, `textureSizeY` | **Always** equal to the PNG pixel width/height; set on load, not free-edited unless regenerating from image |
| `stats` | Full `EnemyStats` |

### Enemy instance part (`EnemyPart` placement)

Configurable in the enemy designer only:

| Field | Notes |
| --- | --- |
| `partName` | References a defined part |
| `offsetX`, `offsetY` | Pixel offsets |
| `offsetZ` | Z / draw order (also used for color calc in-game) |
| `rotation` | **Radians** in files; UI may show degrees |

Game limit: `MAX_ENEMY_PARTS` = **32**. Enforce in the UI.

`textureSizeX/Y` and combined `stats` on placed parts are derived from the part definition (and stats fold), not authored per placement.

### Enemy size

`EnemyDefinition.width` / `height` are authored in **game units** (not pixels). **1 game unit = 32 pixels**. Part offsets remain in pixels and may be placed outside the enemy bounds; the canvas shows a blue rectangle for the enemy size.

### AI (game-side, not in export yet)

Game also has `EnemyAI` (`GRID_APPROACH`, `SHIELD_APPROACH`, `RANGER`). Not included in the `.enemy` format yet.

## Combined stats (must match C exactly)

```c
EnemyStats combineStats(EnemyStats first, EnemyStats second) {
    return (EnemyStats) {
        .health = (first.health + second.health) * second.healthMult,
        .healthMult = 0,
        .speed = (first.speed + second.speed) * second.speedMult,
        .speedMult = 0,
        .action = (first.action + second.action) * second.actionMult,
        .actionMult = 0
    };
}
```

Fold order: start from all-zero stats, then `combineStats(acc, part.stats)` for each part in list order. Show the **final** health / speed / action in the enemy designer (multipliers after combine are 0 — display the three result values).

## File formats

Plain text, one value per line, UTF-8. No blank lines. Floats as plain decimal strings.

### `.part` (v1)

```
v1
<name>
<texture>
<textureSizeX>
<textureSizeY>
<health>
<healthMult>
<speed>
<speedMult>
<action>
<actionMult>
```

Example:

```
v1
debug_core
picus
16
16
1
1
0.2
1
60
1
```

### `.enemy` (v2)

```
v2
<width>
<height>
<partName>
<offsetX>
<offsetY>
<offsetZ>
<rotation>
<partName>
...
```

`width` / `height` are game units (1 unit = 32 px). Then repeat the 5-line part block once per part, in draw/combine order. Rotation in **radians**. Part offsets in **pixels**. No enemy name inside the file — use the filename stem as the enemy id (e.g. `grunt.enemy`).

Readers still accept **v1** (parts only; width/height default to `1`).

## UX requirements

### Part designer

1. **+** opens an image picker; creates a part named from the file stem (editable), sizes from PNG, default stats (match game debug defaults if unsure: health `1`/`1`, speed `0.2`/`1`, action `60`/`1`).
2. List existing parts; select to edit name, texture, stats.
3. Persist to `parts/<name>.part`. Keep the texture path/id consistent with how the game will resolve textures.
4. Changing the PNG updates `textureSizeX/Y` automatically.

### Enemy designer

1. Canvas preview of stacked part sprites at their offsets/rotations; respect **z** as draw order.
2. Add part → pick from loaded `.part` library; place on canvas (drag to set x/y if practical).
3. Per-selected-part inspectors: x, y, z, rotation (and which part definition).
4. Live **final stats** panel using `combineStats`.
5. Save/load `.enemy` under `enemies/`. Cap at 32 parts. Include width/height (game units).

## Agent conventions

- Prefer small, focused modules; do not mix I/O with Dear PyGui draw calls.
- When changing formats, bump the version line (`v2`, …) and keep readers backward-compatible when cheap.
- Any change to `combineStats` or field order must stay bit-for-bit aligned with `../src/enemies.c`.
- Do not commit large binary texture dumps unless needed for a sample; prefer tiny PNGs.
- Target Linux and macOS only; no Windows-specific APIs.

## Out of scope (for now)

- Writing C codegen / patching `enemies.c` automatically
- Runtime hot-reload into the game
- Electron, web-only UI, or mobile
