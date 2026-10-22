# File formats

Enemy Designer writes plain UTF-8 text. One value per line. No blank lines. Floats are decimal strings (`1`, `0.2`, `1.5`). Integers are decimal with no suffix.

For older `.enemy` files, the filename stem is the id. From **v4** onward the enemy name is stored in the file (after the version line); the designer still uses the filename stem when renaming/saving on disk.

| Kind | Extension | Current version | Directory |
| --- | --- | --- | --- |
| Part definition | `.part` | `v1` | `parts/` |
| Enemy composition | `.enemy` | `v5` | `enemies/` |

Writers always emit the current version. Readers keep older `.enemy` versions.

Game limit: at most **32** parts per enemy (`MAX_ENEMY_PARTS`).

Units:

- Part texture sizes and part offsets: **pixels**
- Enemy width / height: **game units** (`1` unit = `32` pixels)
- Rotation: **radians** in files (the UI may show degrees)

Texture ids match the game: the PNG filename stem under `resources/textures/` (for example `picus` for `picus.png`).

---

## `.part` (v1)

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

| Line | Field | Type | Notes |
| --- | --- | --- | --- |
| 1 | version | string | Always `v1` |
| 2 | `name` | string | Tool id; enemies reference this |
| 3 | `texture` | string | Game texture id (PNG stem) |
| 4 | `textureSizeX` | int | PNG pixel width |
| 5 | `textureSizeY` | int | PNG pixel height |
| 6 | `health` | float | Base health |
| 7 | `healthMult` | float | Health multiplier |
| 8 | `speed` | float | Base speed |
| 9 | `speedMult` | float | Speed multiplier |
| 10 | `action` | float | Action cooldown (lower is faster) |
| 11 | `actionMult` | float | Action multiplier |

`textureSizeX` / `textureSizeY` must match the PNG. New parts default to `health 0 / 1`, `speed 0 / 1`, `action 0 / 1`.

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

---

## `.enemy` (v5)

Current write format.

```
v5
<name>
<ai>
<difficulty>
<width>
<height>
<partName>
<offsetX>
<offsetY>
<offsetZ>
<rotation>
<colorR>
<colorG>
<colorB>
<colorA>
<partName>
...
```

Header, then one **9-line** block per placed part, in draw / combine-stats order.

| Line | Field | Type | Notes |
| --- | --- | --- | --- |
| 1 | version | string | `v5` |
| 2 | `name` | string | Enemy display / logical name |
| 3 | `ai` | int | Index into the game's enemy AI array |
| 4 | `difficulty` | float | Enemy difficulty |
| 5 | `width` | float | Enemy size in game units |
| 6 | `height` | float | Enemy size in game units |
| then, per part | `partName` | string | Must match a `.part` `name` |
| | `offsetX` | float | Pixel offset |
| | `offsetY` | float | Pixel offset |
| | `offsetZ` | float | Draw order (also used in-game for color calc) |
| | `rotation` | float | Radians |
| | `colorR` | int | `0`–`255` |
| | `colorG` | int | `0`–`255` |
| | `colorB` | int | `0`–`255` |
| | `colorA` | int | `0`–`255` |

Parts may sit outside the width/height box. The designer draws that box in pixels as `width * 32` by `height * 32`.

### Color

Channels are integers `0`–`255`. Tint is subtractive / multiplicative:

`result = sprite_channel * color_channel / 255`

| Color | Result |
| --- | --- |
| `255 255 255 255` | Sprite appears unchanged |
| `255 0 0 255` | Only red and alpha remain; green and blue become `0` |
| `255 255 255 0` | Fully transparent |

Example:

```
v5
grunt
0
1
1.5
1.5
body
0
0
0
0
255
255
255
255
arm
8
0
0.1
0.785398
255
0
0
255
```

---

## Older `.enemy` versions

Readers still accept these. Missing fields use the defaults below.

### v4

```
v4
<name>
<ai>
<width>
<height>
<partName>
...
```

Same as v5 without `difficulty` (defaults to `0`).

### v3

```
v3
<width>
<height>
<partName>
...
```

Same part blocks as v5 (9 lines, with color). No name / AI / difficulty in file: name defaults to the filename stem, AI and difficulty default to `0`.

### v2

```
v2
<width>
<height>
<partName>
<offsetX>
<offsetY>
<offsetZ>
<rotation>
...
```

Same as v3, but each part is **5 lines** (no color). Loaded color defaults to `255 255 255 255`. Name = filename stem; AI and difficulty = `0`.

### v1

```
v1
<partName>
<offsetX>
<offsetY>
<offsetZ>
<rotation>
...
```

No width/height. Loaded size defaults to `1` × `1` game units. Color defaults to `255 255 255 255`. Name = filename stem; AI and difficulty = `0`.
