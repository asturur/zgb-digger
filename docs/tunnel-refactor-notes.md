# Tunnel Refactor Notes

This document captures what we learned during the latest tunnel refactor work on the split `itemMap` / `tunnelMap` model.

It is a checkpoint note, not a final design spec.

## Current Runtime Model

- Runtime map state is split into two RAM arrays:
  - `itemMap[150]` stores cell occupancy such as emeralds, bags, gold, and reserved bonus cells
  - `tunnelMap[150]` stores tunnel topology and digging progress
- Legacy level data in `Levels.c` still stays in the old compact seed format.
- Level load translates legacy seed bytes into the new runtime maps.

## Tunnel Byte Meaning

- The tunnel byte is currently interpreted as `VVVVHHHH`.
- Low nibble `HHHH` is horizontal digging progress.
- High nibble `VVVV` is vertical digging progress.
- The useful mental model is a `4 x 4` excavation grid inside each `16 x 16` meta-cell:
  - each horizontal bit opens one `4 px` column
  - each vertical bit opens one `4 px` row
  - each `8 x 8` tile therefore spans two horizontal bits and two vertical bits
- Tile-pair masks are:
  - `0x03` left `8 px`
  - `0x0C` right `8 px`
  - `0x30` top `8 px`
  - `0xC0` bottom `8 px`
- Center-band masks are:
  - `0x06` middle `8 px` horizontally
  - `0x60` middle `8 px` vertically
- Straight tunnel segments now stamp the orthogonal center band as well as the travel axis:
  - a full horizontal cell is `0x6F`
  - a full vertical cell is `0xF6`
- A full cross is `0xFF`.
- A corner is the union of one directional half-lane plus the orthogonal center band, for example:
  - left plus up is `0x77`
  - right plus down is `0xEE`

## Tile-To-Bit Mapping

- Top-left tile reads horizontal bits `0x03` and vertical bits `0x30`.
- Top-right tile reads horizontal bits `0x0C` and vertical bits `0x30`.
- Bottom-left tile reads horizontal bits `0x03` and vertical bits `0xC0`.
- Bottom-right tile reads horizontal bits `0x0C` and vertical bits `0xC0`.
- A tile should only be treated as fully open on an axis when both bits for that tile are set on that axis.
- The current load-time render rule is:
  - both axes open for that tile: `tileBlack`
  - only horizontal open: `topWall` or `bottomWall`
  - only vertical open: `leftWall` or `rightWall`
  - neither axis open: the matching corner wall tile
- This is what keeps straight tunnels thin:
  - horizontal travel eventually fills all four horizontal bits and only the two center vertical bits
  - vertical travel eventually fills all four vertical bits and only the two center horizontal bits
- Examples:
  - legacy `3` loads as a horizontal cell with top and bottom walls only
  - legacy `12` loads as a vertical cell with left and right walls only
  - legacy `9` loads as left-plus-up, with `tileBlack` in the top-left and `bottomRightWall` in the bottom-right
  - legacy `5` loads as left-plus-down, with `tileBlack` in the bottom-left and `topRightWall` in the top-right

## Legacy Seed Translation

One important bug found during the refactor:

- The legacy seed format does not use the same bit layout as joypad `J_*` constants.
- The old level encoding is:
  - left = `0x01`
  - right = `0x02`
  - down = `0x04`
  - up = `0x08`
- Using `J_LEFT`, `J_RIGHT`, `J_UP`, `J_DOWN` to decode level seeds silently broke seeded tunnel topology, especially corners.
- Straight tunnels often looked fine by accident, but enemy movement exposed the bad corner mapping quickly.

Rule:

- Keep explicit legacy tunnel seed constants separate from joypad direction constants.

## Banking Lessons

The main fixed-bank lesson from this refactor:

- Any helper that manually calls `SWITCH_ROM(...)` must stay `NONBANKED` unless the whole call path is redesigned.

Confirmed cases:

- `copyLevelMapToRam()` must stay `NONBANKED`
- `copyTileMapToRam()` must stay `NONBANKED`
- `legacySeedToTunnel()` also had to become `NONBANKED` because it is called from the fixed-bank level-load path after switching to the level-data bank

Observed failure mode:

- Debug could still work by luck when a helper happened to land in the same bank as the active level data
- Release could fail with a white screen because the same plain call landed in a different bank

Rule:

- When Release and Debug disagree, inspect banking early before assuming the gameplay logic is wrong.

## Digging Rendering Lessons

The player digging path should not use `renderMetaCell()`.

Why:

- `renderMetaCell()` redraws the full 2x2 meta-cell and is correct for item restoration and whole-cell redraws
- digging progression needs partial updates
- using `renderMetaCell()` while digging caused the whole cell to black out too early

Current direction:

- `SpritePlayer.c` owns the digging-progress renderer
- digging updates only the affected 2-tile strip
- `renderMetaCell()` stays for non-digging redraws such as:
  - item restoration
  - bag removal or replacement
  - gold consumption
  - whole-cell state refresh

## Thin-Wall Digging Tiles

Current temporary tunnel visuals use these tiles:

- `leftWall`
- `rightWall`
- `topWall`
- `bottomWall`

Current behavior:

- vertical digging uses `leftWall` and `rightWall`
- horizontal digging uses `topWall` and `bottomWall`
- a strip falls back to `tileBlack` only when that 8x8 tile was already open in the orthogonal direction

This is intentionally a temporary visual rule until more tunnel tiles are available.

## Item Pickup Timing

Another asymmetry found during the refactor:

- `runMapSideEffects()` originally used the player's top-left meta-cell
- that made left and up feel correct, but right and down lagged by one cell

Current rule:

- item-side effects use the player's leading meta-cell, matching the digging logic

Result:

- item pickup timing is now symmetric across all four directions

## Enemy Movement Lessons

Enemy movement was sensitive to tunnel interpretation in two separate ways:

1. Legacy seeded corners broke when seed translation used joypad constants instead of explicit legacy seed bits.
2. A too-strong destination-cell rule blocked enemies from entering valid curve cells.

What worked best so far:

- keep enemy legality based on actual tunnel exits
- also require the destination cell to be open on the entrance half that the enemy is entering through

Current entrance-half rule:

- moving left: destination cell must expose the right half of its horizontal lane
- moving right: destination cell must expose the left half of its horizontal lane
- moving up: destination cell must expose the bottom half of its vertical lane
- moving down: destination cell must expose the top half of its vertical lane

This prevents enemies from entering only-partially-open cells while still allowing 90-degree turns.

## Open Design Question

There is still an unresolved design disagreement about how movement should interpret corners.

Current implementation:

- movement follows the same partial-half model as the dig-progress encoding
- a corner is represented as one horizontal half plus one vertical half

Alternative interpretation discussed:

- once a cell becomes a completed turn, movement could treat it as fully open in both relevant directions, even if rendering still uses finer-grained progress bits

This should be revisited when the final tunnel tile set is available.

## What Is Intentionally Deferred

- final proper turn tiles
- final tee and cross tiles
- a more polished render pipeline for tunnel topology
- a final decision on whether movement should use the raw progress bits or a normalized topology view
- further visual refinement of digging strips and overlaps

## Practical Rules For Future Work

1. Keep `itemMap` and `tunnelMap` as the authoritative runtime state.
2. Do not use joypad direction constants to decode legacy level seed tunnel bits.
3. Keep fixed-bank level-load helpers `NONBANKED` when they switch ROM banks.
4. Keep partial digging rendering local to the player path instead of routing it through `renderMetaCell()`.
5. Treat enemy movement and tunnel rendering as related but not identical concerns; visual progress and movement legality may need different abstractions.
