# Bag/Gold Bug Investigation Plan

## Scope

This plan is only for the bag-to-gold crash path.

Out of scope for this round:

- bag pushing
- enemy behavior
- emulator setup beyond what is needed to debug this crash
- refactors unrelated to the crash

## Current Context

Relevant files:

- `ZGB-template-master/src/SpriteBag.c`
- `ZGB-template-master/src/SpriteGold.c`
- `ZGB-template-master/src/StateGame.c`

Current transition:

1. A falling bag lands in `SpriteBag.c`
2. If `bagFallCounter > 40`, it does `SpriteManagerAdd(SpriteGold, THIS->x, THIS->y)`
3. The bag sprite is removed
4. `SpriteGold.c` starts with `goldStatus = undefined`
5. `SpriteGold.c` decides whether to crumble into background gold or to fall

## Main Hypotheses

### Hypothesis 1: bottom-edge out-of-bounds read

Most likely first bug.

Risky code in `SpriteGold.c`:

- `row = TILE_FROM_PIXEL(THIS->y) + 2`
- `checkTilesFor(column, row, tileBlack)` is evaluated before checking the bottom map bound

If `row` is already at the bottom, `checkTilesFor()` reads `row + 1`, which can go out of bounds.

### Hypothesis 2: stale state after `undefined`

`SpriteGold.c` reads:

- `uint8_t state = THIS->custom_data[goldStatus];`

Then inside the `undefined` branch it changes `goldStatus`, but the local `state` variable is not refreshed before later checks.

This can cause the first gold update after spawn to run the wrong logic or skip the intended branch.

### Hypothesis 3: `stateFalling` is incomplete

`SpriteGold.c` can set:

- `goldStatus = stateFalling`

But there is no active falling update path implemented afterward.

That is not necessarily the first crash source, but it makes the transition incomplete and suspect.

## Investigation Order

### Step 1: reproduce with debugger

Use the working Emulicious + VS Code debug configuration.

Breakpoints:

- `ZGB-template-master/src/SpriteBag.c` on the line that calls `SpriteManagerAdd(SpriteGold, THIS->x, THIS->y)`
- `ZGB-template-master/src/SpriteGold.c` at `START()`
- `ZGB-template-master/src/SpriteGold.c` at the top of `UPDATE()`

Values to inspect:

- `THIS->x`
- `THIS->y`
- `THIS->custom_data[goldStatus]`
- `THIS->custom_data[crumblingTimer]`
- local `row`
- local `column`

Goal:

- confirm exact coordinates where gold is created
- confirm whether the crash happens inside the first `SpriteGold` update

### Step 2: test the bounds-check patch only

First fix to try:

- in `SpriteGold.c`, check the bottom bound before calling `checkTilesFor()`
- also guard `row < 22` before probing the 2x2 tile block below

This should be the first patch because it is small and directly targets the most likely crash.

### Step 3: refresh `state` after leaving `undefined`

Second fix to try:

- after assigning `THIS->custom_data[goldStatus]` in the `undefined` branch, refresh the local `state`

This is also a small patch and keeps the current structure.

### Step 4: add a minimal safe `stateFalling` behavior

If the crash is gone but the gold behavior is still broken:

- implement the smallest safe `stateFalling` path
- either make it transition immediately to background gold or add the real falling logic later

For the first pass, prefer the smallest safe behavior, not a full gold-feature port.

## Patch Sequence

Apply fixes in this order, testing after each:

1. `SpriteGold.c` bounds-check order
2. `SpriteGold.c` refresh local `state`
3. minimal `stateFalling` behavior

Do not patch `SpriteBag.c` unless the debugger proves the crash happens before `SpriteGold.c` is entered.

## Debug Session Script

For each run:

1. build a debug ROM
2. launch the debugger
3. reproduce one bag-to-gold conversion
4. stop at the `SpriteBag -> SpriteGold` handoff
5. step into `SpriteGold START`
6. step into the first `SpriteGold UPDATE`
7. record `x`, `y`, `row`, `column`, `goldStatus`

## Success Criteria

The bug is considered fixed when all of the following are true:

- a falling bag can convert to gold without crashing
- the gold sprite or background transition completes consistently
- the map does not corrupt during the transition
- the fix stays limited to the bag/gold path

## Non-Goals For This Plan

Do not combine this work with:

- bag pushing
- gold gameplay redesign
- enemy interactions
- cleanup refactors

Keep each change small enough to isolate regressions.
