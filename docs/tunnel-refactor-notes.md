# Tunnel Refactor Notes

This document captures what we learned during the latest tunnel refactor work on the split `itemMap` / `tunnelMap` model.

It is a checkpoint note, not a final design spec.

## Current Runtime Model

- Runtime map state is split into two RAM arrays:
  - `itemMap[150]` stores cell occupancy such as emeralds, bags, gold, and reserved bonus cells
  - `tunnelMap[150]` stores tunnel topology and digging progress
- `Levels.c` now stores direct tunnel bytes for open cells and `0` for grass.
- Reserved unreachable bytes such as `EMR` and `BAG` are used as item sentinels in the level data.
- Level load copies tunnel bytes directly into `tunnelMap` and resolves item sentinels into `itemMap`.

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
  - only horizontal open: `tileTopWall` or `tileBottomWall`
  - only vertical open: `tileLeftWall` or `tileRightWall`
  - neither axis open: the matching corner wall tile
- This is what keeps straight tunnels thin:
  - horizontal travel eventually fills all four horizontal bits and only the two center vertical bits
  - vertical travel eventually fills all four vertical bits and only the two center horizontal bits
- Examples:
  - `HHH` loads as a horizontal cell with top and bottom walls only
  - `VVV` loads as a vertical cell with left and right walls only
  - `CTL` loads as left-plus-up, with `tileBlack` in the top-left and `tileBottomRightWall` in the bottom-right
  - `CBL` loads as left-plus-down, with `tileBlack` in the bottom-left and `tileTopRightWall` in the top-right

## Progress Helper Semantics

The shared helper `extendTunnelProgressAt(cell, direction, slotIndex)` has two different phases.

### Phase 1: Straight Progress From The Entry Edge

- This is the normal case for a fresh cell.
- Each direction uses a 4-entry table that opens the lane from the boundary the digger entered from up to `slotIndex`.
- The four tables are:
  - `horizontalProgressMasksRight`
  - `horizontalProgressMasksLeft`
  - `verticalProgressMasksDown`
  - `verticalProgressMasksUp`

Examples:

- moving right uses `0x01`, `0x03`, `0x07`, `0x0F`
- moving left uses `0x0F`, `0x0E`, `0x0C`, `0x08`
- moving down uses `0x10`, `0x30`, `0x70`, `0xF0`
- moving up uses `0xF0`, `0xE0`, `0xC0`, `0x80`

### Phase 2: Branching Or Turning From An Already-Open Center

- Once a cell already has the axis center band open, `slotIndex` no longer means "keep opening from the far boundary".
- On a turn or branch, the desired result is only to stamp the half-lane on the side being exited toward.
- If we reused the phase-1 table in this state, a turn could wrongly reopen the opposite half of the cell and create the wrong tee orientation.

Examples:

- from a horizontal cell, turning up should add only `tunnelVerticalStep12`, not replay vertical progress from the bottom edge
- from a vertical cell, turning right should add only `tunnelHorizontalStep34`, not replay horizontal progress from the left edge

Rule:

- before the center band exists on that axis, use the per-direction progress table
- after the center band exists on that axis, stamp only the directional half-lane for the turn or branch

## Banking Lessons

The main fixed-bank lesson from this refactor:

- Any helper that manually calls `SWITCH_ROM(...)` must stay `NONBANKED` unless the whole call path is redesigned.

Confirmed cases:

- `copyLevelMapToRam()` must stay `NONBANKED`
- `copyTileMapToRam()` must stay `NONBANKED`

Observed failure mode:

- Debug could still work by luck when a helper happened to land in the same bank as the active level data
- Release could fail with a white screen because the same plain call landed in a different bank

Rule:

- When Release and Debug disagree, inspect banking early before assuming the gameplay logic is wrong.

## Digging Rendering Lessons

The current tunnel refactor moved visual ownership back to `renderMetaCell()`.

Current direction:

- digging code is responsible for updating `tunnelMap`
- `renderMetaCell()` is responsible for turning tunnel bits into the final 2x2 tile quartet
- both the cell being entered and the cell being exited must be rendered after their tunnel bits are updated

This is simpler than the old partial-strip renderer, but it makes the tunnel-bit semantics more important because wrong bits now redraw the whole meta-cell incorrectly.

## Thin-Wall Digging Tiles

## Transition Bug Findings

These findings capture the stale exiting-cell bug that showed up during the refactor, especially while turning from a horizontal tunnel into an upward tunnel. The bug is useful to keep documented because it explains why the progress helper needs separate "fresh progress" and "branch/turn" behavior.

### Symptom

- When Digger exits a meta-cell, the cell being left behind can keep an incorrect wall strip.
- A reproducible example is turning upward out of a horizontal tunnel:
  - the junction should become a tee
  - instead, the old cell can keep a right-side dirt strip rendered as `tileTopWall`

### Root Cause

- The problem is not missing redraw calls.
- The problem is stale `tunnelMap` bits on the cell being exited.

In the current player digging path:

- `updateTunnelProgress()` in `SpritePlayer.c` already calls:
  - `renderMetaCell(previousDigCell)` when the leading edge moves into a new meta-cell
  - `renderMetaCell(currentDigCell)` after extending the new leading cell
- However, before redrawing `previousDigCell`, the code does not update that old cell with the new exit connection bits.

So the sequence is currently:

1. Leading edge enters the next meta-cell.
2. Only `currentDigCell` gets new tunnel progress bits through `extendTunnelProgress()`.
3. `previousDigCell` is redrawn without receiving the exit bits for the new branch direction.
4. The old cell is rendered from stale topology and stays visually wrong.

### Exact Wrong Location

Player path:

- `ZGB-template-master/src/SpritePlayer.c`
- `updateTunnelProgress()`

Relevant behavior:

- `previousDigCell` is only rendered on cell change.
- `currentDigCell` is the only cell whose tunnel bits are extended.

Enemy Hobbin path has the same structural issue:

- `ZGB-template-master/src/SpriteEnemy.c`
- `updateEnemyTunnelProgress()`

There too:

- `previousDigCell` is only rendered
- `currentDigCell` is the only cell whose tunnel bits are extended

### Why Rendering Alone Is Not The Fix

- Calling `renderMetaCell()` on both cells is already happening in the player transition path.
- Adding more redraw calls without changing `tunnelMap` would redraw the same stale topology.
- So the simplest correct fix direction is to update the exiting cell's tunnel bits first, then redraw it.

### Existing Helper That Matches The Correct Shape

- `openTunnelConnection()` in `StateGame.c`

This helper already follows the correct pattern:

1. update tunnel bits for both connected cells
2. apply center-mask promotion for both cells
3. redraw both cells

That is the right conceptual model for fixing player and Hobbin transition updates as well.

Current temporary tunnel visuals use these tiles:

- `tileLeftWall`
- `tileRightWall`
- `tileTopWall`
- `tileBottomWall`

Current behavior:

- vertical digging uses `tileLeftWall` and `tileRightWall`
- horizontal digging uses `tileTopWall` and `tileBottomWall`
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

1. Tunnel bits must stay consistent with the actual `VVVVHHHH` digging model and rendered cell shapes.
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
2. Keep fixed-bank level-load helpers `NONBANKED` when they switch ROM banks.
3. For fresh digging, interpret `slotIndex` as progress from the entry boundary toward the center.
4. For turns and branches on an already-open center band, stamp only the half-lane on the exit side instead of replaying boundary progress.
5. Treat enemy movement and tunnel rendering as related but not identical concerns; visual progress and movement legality may need different abstractions.
