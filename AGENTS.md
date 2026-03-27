# AGENTS.md

## Project Mission

This repository is a Game Boy reimplementation / port of **Digger** for the **original Nintendo Game Boy** using the **ZGB** SDK.

The working goal is not to preserve the PC code structure verbatim. The goal is to reproduce Digger's gameplay, level logic, and feel within original Game Boy constraints:

- 160x144 visible resolution
- 4-shade monochrome graphics
- tile and sprite based rendering
- tight CPU and ROM/RAM limits

When making changes, prioritize faithful gameplay over line-by-line source translation.

## Repository Map

### Active game project

`ZGB-template-master/`

This is the codebase that builds the Game Boy ROM. Most gameplay changes should land here.

Important areas:

- `ZGB-template-master/src/`
  Game logic, state flow, sprite behavior, level data, and build entry point.
- `ZGB-template-master/include/`
  Shared constants and public declarations used by the gameplay code.
- `ZGB-template-master/res/`
  Tilemaps, tilesets, fonts, and other source assets consumed by the build.
- `ZGB-template-master/bin/gb/Digger.gb`
  Expected ROM output after a successful `make gb`.

### Reference-only source material

`digger_pc_src/`

This contains the original / remastered PC Digger sources from Windmill Software and related assets. Use it as a **behavioral and content reference**, especially for:

- level layouts
- score rules
- enemy behavior
- bag and gold timing
- movement quirks
- sound and progression rules

Do not treat `digger_pc_src/` as the place to implement the Game Boy port unless the task is specifically about studying or documenting the original behavior.

### Vendored SDK and tools

`ZGB-2023.0/`

This is a vendored ZGB snapshot used by the game project. Avoid editing it unless the task is clearly an SDK/toolchain fix that cannot live in the game code.

### Historical / backup assets

- `assets_backup/`
- `digger.png`

Useful as references, but not the primary source of truth for runtime logic.

## Build And Tooling

The current game project is configured to build from:

- `ZGB-template-master/src/Makefile`

Key details already encoded there:

- project name: `Digger`
- target platform: `gb`
- SDK path defaults to the vendored `../../ZGB-2023.0/common`

Expected build command:

```sh
cd /Users/andreabogazzi/develop/zgb-digger/ZGB-template-master/src
make gb
```

Prerequisite:

- `gbdk-2020` must be installed and discoverable either through `GBDK_HOME` or at `ZGB-2023.0/env/gbdk`
- Java is required to launch the local `Emulicious/Emulicious.jar`

If you change build assumptions, keep them consistent with the vendored SDK layout already used by the repo.

## Current Implementation Status

The Game Boy port is already beyond template stage. Current code in `ZGB-template-master/` includes:

- a playable `StateGame`
- level loading for five levels
- custom compact level encoding in `src/Levels.c`
- digging / tunnel carving logic
- emerald collection and score handling
- lives and HUD updates
- bag activation and falling behavior
- player shooting with a fireball sprite

This means new work should usually extend or correct existing gameplay code, not replace the project with a fresh ZGB sample.

## Key Files For Gameplay Work

- `ZGB-template-master/src/StateGame.c`
  Main game loop, level loading, score/lives state, HUD updates, and tilemap RAM handling.
- `ZGB-template-master/src/SpritePlayer.c`
  Digger movement, digging, direction state, death animation, and firing.
- `ZGB-template-master/src/SpriteBag.c`
  Falling bag behavior and conversion to gold.
- `ZGB-template-master/src/Levels.c`
  Encoded level layouts derived from the original game.
- `ZGB-template-master/include/StateGame.h`
  Map dimensions, tile/meta-tile constants, scores, bounds, and gameplay macros.
- `digger_pc_src/remaster/main.c`
  Canonical reference for original level data and top-level game flow.
- `digger_pc_src/remaster/digger.c`
  Canonical reference for Digger movement, firing, bonus mode, timing, and player state.

## Porting Rules

1. Change gameplay code in `ZGB-template-master/` first.
2. Use `digger_pc_src/` to verify original behavior before changing mechanics.
3. Keep Game Boy limitations in mind before copying PC behavior directly.
4. Prefer compact data representations. The current level format is deliberately smaller than a full tilemap.
5. Preserve the existing distinction between:
   - meta-level logic in `levelMap`
   - rendered 8x8 background tiles in `tileMap`
6. Be cautious with anything that touches ROM banking, `NONBANKED` functions, or large arrays copied into RAM.
7. Avoid changing `ZGB-2023.0/` unless a repo task explicitly requires SDK-level changes.

## Banking Notes

- `ZGB-template-master/src/Makefile` uses `N_BANKS = A`, so the linker may rebalance code and assets across ROM banks as the project grows.
- Files that include `Banks/SetAutoBank.h` or use `#pragma bank 255` should be treated as autobanked modules.
- Prefer `BANKED` shared helpers over `NONBANKED` shared helpers when the helper really must be cross-file and does not need to be in fixed bank `0`.
- In practice, an autobanked sprite or state file should be treated as an island:
  - calling `static` helpers in the same `.c` file is safe
  - calling ZGB engine entry points that switch banks on purpose is safe
  - calling your own `BANKED` helpers is safe when they are declared and used as `BANKED`
  - calling your own `NONBANKED` helpers is safe
  - calling ordinary cross-file gameplay helpers is unsafe unless you have explicitly designed the bank switch
- ZGB switches banks when entering sprite `START/UPDATE/DESTROY` handlers and state `START/UPDATE` handlers. In addition, `BANKED` functions are bank-safe call targets because the runtime/compiler handles the bank switch. Plain C calls between autobanked gameplay files are not automatically banked unless the target is actually `BANKED`.
- A bug confirmed during bag-to-gold work: `SpriteGold` was placed in ROM bank `02` while shared helpers in `StateGame.c` such as `updateVideoMemAndMap()`, `checkTilesFor()`, and `addOnMap()` were in ROM bank `01`. Direct calls from `SpriteGold` to those helpers crashed until the shared helpers were moved to `NONBANKED`.
- Do not rely on two gameplay files staying in the same bank just because the current build happens to place them together. With `N_BANKS = A`, that can change after unrelated code or asset growth.
- Prefer this structure for new gameplay code:
  - keep sprite-private or state-private logic as `static` functions inside the owning `.c`
  - expose only small shared gameplay services through headers
  - default shared cross-file gameplay services to `BANKED`
  - use `NONBANKED` only when the helper must live in fixed bank `0`, is performance-sensitive enough to justify it, is needed in contexts where banked calls are awkward, or is part of a small always-available runtime surface
- `NONBANKED` should be treated as a last-resort or special-case tool, not the default answer to every banking problem.
- Good candidates for `NONBANKED` are small fixed-bank runtime helpers, hot tile/map primitives, ISR-adjacent code, and functions that must remain callable regardless of the currently mapped bank.
- Be especially careful when moving helpers from a sprite file into `StateGame.c` or another shared module. That can accidentally convert a same-file call into a cross-bank call.

## Behavioral Reference Notes

Some useful correspondences already visible in the repo:

- The five encoded Game Boy levels in `ZGB-template-master/src/Levels.c` mirror the first five PC layouts from `digger_pc_src/remaster/main.c`.
- The Game Boy map uses 15x10 meta-cells, where each meta-cell expands to 2x2 background tiles.
- Digging updates both VRAM and the in-memory `tileMap`, while progression logic operates on `levelMap`.

When implementing missing mechanics, compare against the PC source for:

- enemy spawn cadence and per-level enemy count
- fireball recharge timing
- bag falling thresholds
- bonus mode
- level completion rules
- death and respawn flow

## Enemy Porting Notes

- The original PC game runs at about 15 Hz, while the Game Boy game loop is about 60 Hz. For rough timing ports, `1` original game tick is approximately `4` GB frames.
- The current port uses a compact tunnel encoding in `levelMap`:
  - `1` left
  - `2` right
  - `4` down
  - `8` up
- Do not mix the tunnel bit encoding with GBDK joypad direction constants (`J_LEFT`, `J_RIGHT`, `J_UP`, `J_DOWN`). They are different domains even when some values happen to overlap.
- Macro arguments in `StateGame.h` must stay fully parenthesized. Unparenthesized pixel/tile macros caused SDCC optimizer warnings and broke alignment checks in enemy movement work.
- In `SpriteEnemy.c`, movement legality must account for both:
  - tunnel openness from `levelMap`
  - map bounds
  If only tunnel bits are checked, enemies can freeze or try to leave the map at corners and edges.
- Keep direction-choice legality and actual movement behavior aligned. If one layer allows a direction that the movement step later refuses, enemies can stall at cell boundaries.
- `Sprite.custom_data` only has `8` bytes in this setup. Prefer mode-dependent reuse of slots before adding new enemy state:
  - current enemy work uses `mode` and `mode_timer`
  - `mode_timer` is currently reused for wait/death/Nobbin-Hobbin timing
  - if original monster time-penalty `t` is ported later, it should use a separate slot
- Current `SpriteEnemy.c` behavior is still a simplified port:
  - spawn wait is implemented
  - tunnel-following is implemented
  - Nobbin/Hobbin timing thresholds are partially implemented
  - full chase logic toward Digger is not implemented yet
  - Hobbin digging-through-dirt and original bag interactions are not implemented yet
- The original level completion rule is `all emeralds collected OR all enemies cleared`. In this port, that currently maps to:
  - `diamonds == 0`
  - or `enemySpawned == enemyMaxTotal && enemyCountOnScreen == 0`

## Emulator And Debug Notes

- Prefer Emulicious over SameBoy for this repository.
- The local emulator copy lives at `Emulicious/`, and normal runs can use `java -jar ~/develop/zgb-digger/Emulicious/Emulicious.jar ~/develop/zgb-digger/ZGB-template-master/bin/gb/Digger.gb`.
- For source-level debugging with the VS Code Emulicious extension, use a Debug build:
  - `cd /Users/andreabogazzi/develop/zgb-digger/ZGB-template-master/src`
  - `make BUILD_TYPE=Debug gb`
- The debug outputs are:
  - `ZGB-template-master/bin/gb/Digger_Debug.gb`
  - `ZGB-template-master/bin/gb/Digger_Debug.cdb`
  - `ZGB-template-master/bin/gb/Digger_Debug.sym`
- The working VS Code files are at the repository root:
  - `.vscode/tasks.json`
  - `.vscode/launch.json`
- The older `ZGB-template-master/.vscode` files are template leftovers and point at wrong ROM paths for this repository.
- In the Emulicious VS Code launch configuration, omit the `system` field. The extension rejected `"gb"` with `IllegalArgumentException : No enum constant ... EmulationSystem.GB`.

## Where Agents Should Be Careful

- The repository contains vendored upstream code and reference dumps; not every directory is meant to be edited.
- There are backup files and macOS `.DS_Store` files in the tree; ignore them unless the task is cleanup.
- If a behavior seems odd in the Game Boy code, verify whether it is an intentional simplification before “fixing” it.
- Keep docs and comments grounded in this repo's actual layout: the active project root is `ZGB-template-master/`, not the repository root.

## Preferred Workflow For Future Tasks

1. Identify the gameplay feature in the active Game Boy project.
2. Cross-check the equivalent behavior in `digger_pc_src/`.
3. Implement the smallest Game Boy-friendly change in `ZGB-template-master/`.
4. Rebuild with `make gb` when the toolchain is available.
5. Keep the port playable on real original-Game-Boy constraints, not only in theory.
