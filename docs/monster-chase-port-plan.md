# Monster Chase Port Plan

This note turns the original PC monster chase logic into a concrete implementation plan for the Game Boy port in `ZGB-template-master/`.

It is a planning document only. No behavior is changed here.

## Reference Sources

- Original chase logic:
  - `/Users/andreabogazzi/develop/zgb-digger/digger_pc_src/remaster/monster.c`
  - especially `monai()` around lines `145..243`
  - `fieldclear()` around lines `455..476`
- Current Game Boy enemy logic:
  - `/Users/andreabogazzi/develop/zgb-digger/ZGB-template-master/src/SpriteEnemy.c`
  - especially `chooseEnemyDirection()` around lines `337..374`
- Current player death entry points:
  - `/Users/andreabogazzi/develop/zgb-digger/ZGB-template-master/src/SpritePlayer.c`
  - `/Users/andreabogazzi/develop/zgb-digger/ZGB-template-master/include/SpritePlayer.h`

## What The Original Game Actually Does

The original chase decision happens only when a monster is exactly aligned to a cell:

- `monai()` only recalculates direction when `xr == 0 && yr == 0`
- the target is the monster's assigned `chase` digger
- if that digger is dead, the target flips to the other digger

Direction priorities are built from the relative distance to Digger:

- compare `abs(diggery - mony)` against `abs(diggerx - monx)`
- if vertical distance is strictly larger, vertical becomes priority `1`
- otherwise horizontal becomes priority `1`
- this means ties favor horizontal movement in the original code

The original routine produces four ordered priorities:

1. move toward Digger on the dominant axis
2. move toward Digger on the secondary axis
3. move away on the secondary axis
4. move away on the dominant axis

Then it adjusts that order:

- in bonus mode, priorities are inverted so monsters run away
- reversing direction is demoted to the end unless it is unavoidable
- on levels below `6`, a random branch occasionally swaps priority `1` and `3`

Final choice differs by monster mode:

- Nobbin: choose the first legal direction from the ordered list using `fieldclear()`
- Hobbin: ignore tunnel legality and force priority `1`

The original also increments a separate time-penalty counter when a monster changes direction. That counter contributes to Nobbin to Hobbin conversion.

## Current GB Gap

The current Game Boy enemy AI is still corridor-following, not Digger-chasing:

- `chooseEnemyDirection()` only reacts when the current direction is blocked
- it prefers perpendicular movement based on current travel axis, not on player position
- it only falls back to reverse as a last resort
- it never builds the original 4-way priority list from the player delta

So current monsters can move through tunnels, but they do not pursue Digger with the original axis-priority behavior.

Related gaps:

- there is no current bonus-mode reversal path in the GB port
- there is no current low-level random priority swap
- the current `mode_timer` byte is overloaded for wait timing, Hobbin duration, and simplified Nobbin-to-Hobbin promotion
- enemy code does not currently have a bank-safe shared entry point to trigger player death on monster contact

## Porting Constraints

Important repo-specific constraints before implementing:

- `SpriteEnemy.c` is an autobanked gameplay module
- plain cross-file helper calls are unsafe unless the target is `BANKED` or `NONBANKED`
- chase decision helpers should stay `static` inside `SpriteEnemy.c`
- any shared player-kill helper called from enemies must be exposed explicitly as `BANKED`

Also keep the coordinate model straight:

- original code compares sprite pixel positions, not tile indices
- the GB port should do the same using `scroll_target->x/y` and `THIS->x/y`
- turn decisions should still happen only on exact meta-cell alignment via `isEnemyAligned()`

## Recommended Implementation Plan

### Phase 1: Add A Real Chase Priority Builder

Inside `SpriteEnemy.c`, replace the current fallback-only turn chooser with a helper that:

- reads the active player from `scroll_target`
- bails out cleanly if there is no live player sprite
- compares absolute horizontal and vertical distance in pixels
- reproduces the original tie rule:
  - vertical is primary only when `abs(dy) > abs(dx)`
  - otherwise horizontal is primary
- produces ordered priorities `p1..p4`
- demotes reverse direction to the end using the same reshuffle logic as the PC source

This helper should remain `static` in `SpriteEnemy.c`.

### Phase 2: Separate Nobbin And Hobbin Choice

After the priority list exists:

- Nobbin should choose the first legal move from `p1..p4`
- legality should continue to use the existing GB tunnel/bounds checks in `enemyCanMove()`
- Hobbin should ignore tunnel legality and take `p1` directly, subject only to map-edge safety

This keeps the existing GB tunnel representation and only swaps in the original choice policy.

Implementation detail:

- keep `enemyCanMove()` as the Nobbin legality test
- add a tiny edge-only helper for Hobbin so it can still avoid leaving the map

### Phase 3: Split The Enemy Timers Properly

The original logic uses more than one timer/counter:

- spawn delay / start delay
- Hobbin lifetime counter `hnt`
- direction-change penalty `t`

The GB port currently overloads `mode_timer` for unrelated purposes. That is too compressed for an original-style chase port.

Recommended byte ownership in `Sprite.custom_data`:

- keep `mode` at slot `0`
- keep `mode_timer` at slot `1` for wait and Hobbin duration
- repurpose slot `4` from `frightned` into a dedicated enemy chase penalty counter
- keep movement accumulator and last dig cell as they are

Use the new penalty counter for Nobbin to Hobbin promotion instead of reusing `mode_timer`.

Initial scope for penalty increments:

- increment when a Nobbin changes direction at an aligned turn
- increment when a Nobbin is forced into reverse because its chosen path is blocked by a bag push result

Later, if needed for closer fidelity, also increment on:

- monster-monster collisions
- bag collisions that consume extra time in the PC logic

### Phase 4: Add The Original Low-Level Randomness

The original code occasionally swaps priorities `1` and `3` on levels below `6`.

Recommended GB version:

- add a tiny deterministic RNG state in gameplay code
- only apply the swap when the current level is below `6`
- keep the random branch in the aligned-turn decision path only

This should be implemented after the base chase is working, not before.

Reason:

- without the main priority builder, the random swap is noise
- with the priority builder in place, the swap becomes a small fidelity adjustment

### Phase 5: Hook Enemy Contact To Player Death

Original monsters kill Digger on contact outside bonus mode. The GB port should expose a bank-safe player death entry point for enemy contact.

Recommended structure:

- add a new `BANKED` helper in `SpritePlayer.c`
- declare it in `SpritePlayer.h`
- keep the actual player death setup owned by the player module
- call that helper from `SpriteEnemy.c` after movement and collision checks

Do not call plain player-local helpers across files.

This is the main banking-sensitive part of the chase port.

### Phase 6: Verify Hobbin Digging Still Matches The Existing Tunnel Model

The chase change should not break the current Hobbin digging work:

- Hobbin still needs to update tunnel progress while moving
- when Hobbin reverts to Nobbin, the last partially dug cell still needs final redraw
- forced `p1` movement must not skip the existing dig-progress update path

This phase is mostly regression checking around:

- `updateEnemyTunnelProgress()`
- bag push interactions
- gold consumption

## Suggested Order Of Actual Code Changes

1. Add static helpers in `SpriteEnemy.c` to compute target position and build `p1..p4`
2. Replace `chooseEnemyDirection()` with aligned-cell chase selection
3. Add a dedicated penalty counter byte and stop reusing `mode_timer` for Nobbin promotion
4. Add a `BANKED` player-death helper callable from enemies
5. Add the optional low-level random swap for levels below `6`
6. Rebuild and verify bank placement if any shared helper was added

## Verification Checklist

After implementation, verify these behaviors explicitly:

- when the player is above and farther vertically than horizontally, monsters choose `up` first
- when horizontal and vertical distances are equal, monsters choose horizontal first
- monsters avoid immediate reversal unless no other legal path exists
- Nobbins stay inside existing tunnels
- Hobbins cut through dirt toward the target direction
- Hobbin to Nobbin reversion still happens after roughly `30 + 2 * difficultyLevel` original-style converted frames already used by the port
- enemy touching Digger starts the player death path
- no white-screen or lockup appears from new cross-file calls

## Deliberate Non-Goals For This Pass

This plan is only for monster chase behavior. It does not try to finish every remaining monster mechanic at once.

Out of scope for this pass:

- bonus mode implementation
- two-player chase target assignment
- full monster-monster collision parity
- every original time-penalty source
- exact playback-deterministic RNG matching the remaster

Those can follow after the base chase behavior feels correct on the Game Boy port.
