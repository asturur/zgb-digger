# Issue 25: Digger Death Animation And Audio Spec

This note turns the original DOS / remaster behavior into a Game Boy oriented implementation target for `ZGB-template-master/`.

Reference sources:

- `/Users/andreabogazzi/develop/zgb-digger/digger_pc_src/original/Work/TC.C`
- `/Users/andreabogazzi/develop/zgb-digger/digger_pc_src/original/Work/TH.C`
- `/Users/andreabogazzi/develop/zgb-digger/digger_pc_src/original/Work/DIGGER.H`
- `/Users/andreabogazzi/develop/zgb-digger/digger_pc_src/remaster/digger.c`
- `/Users/andreabogazzi/develop/zgb-digger/digger_pc_src/remaster/sound.c`
- `/Users/andreabogazzi/develop/zgb-digger/digger_pc_src/remaster/mkg.c`

## Current GB Gap

Current player death in the GB port is still placeholder behavior:

- `SpritePlayer.c` uses a single-frame dead sprite `anim_dead = {1, 24}`.
- `killPlayer()` immediately decrements lives and starts a short countdown.
- `UPDATE()` only performs a small vertical hop before removing the sprite.
- `StateGame.c` freezes the round with `isDying`, but there is no original-style staged death sequence and no dedicated death tune.

Files involved now:

- `/Users/andreabogazzi/develop/zgb-digger/ZGB-template-master/src/SpritePlayer.c`
- `/Users/andreabogazzi/develop/zgb-digger/ZGB-template-master/src/StateGame.c`
- `/Users/andreabogazzi/develop/zgb-digger/ZGB-template-master/include/SpritePlayer.h`

## Original Death Modes

The original game has two visible death paths:

1. Bag death
2. Bug death

Both end in the same grave sequence.

### Bag death

Original flow:

1. Digger is held in a crushed pose while the falling bag is still descending.
2. When the bag stops, the death sweep sound starts.
3. Wait `5` original ticks.
4. Move Digger up `6` pixels.
5. Show grave frames `grave5 -> grave4 -> grave3 -> grave2 -> grave1`.
6. Wait `2` original ticks between grave frame changes.
7. Hold the last grave frame for `60` ticks if music is enabled, otherwise `10`.
8. Mark the player dead and continue with life loss / respawn.

Reference:

- `dead_move()` in `/Users/andreabogazzi/develop/zgb-digger/digger_pc_src/original/Work/TC.C`
- `diggerdie()` in `/Users/andreabogazzi/develop/zgb-digger/digger_pc_src/remaster/digger.c`

### Bug death

Original flow:

1. Digger enters a short bounce arc using the crushed pose.
2. Bounce offsets by step are `3, 5, 6, 6, 5, 3, 0`.
3. On the first bounce step the death sweep sound starts.
4. After the bounce, wait `5` ticks.
5. Reuse the same grave sequence as bag death.

Reference:

- `BOUNCE` in `/Users/andreabogazzi/develop/zgb-digger/digger_pc_src/original/Work/TC.C`
- `deatharc[]` and `case 5` in `/Users/andreabogazzi/develop/zgb-digger/digger_pc_src/remaster/digger.c`

## Timing Conversion For GB

The old game runs at about 15 gameplay frames per second. For this project we have already been using:

- `1 original tick ~= 4 Game Boy frames`

Use that same conversion for issue 25 so the death sequence stays consistent with the rest of the port.

### Recommended GB timings

- Bag pre-grave wait: `5` old ticks -> `20` GB frames
- Grave frame hold: `2` old ticks -> `8` GB frames per grave frame
- Bug bounce: `7` old ticks -> `28` GB frames total
- Final corpse hold with music: `60` old ticks -> `240` GB frames
- Final corpse hold without music: `10` old ticks -> `40` GB frames

The bounce arc can stay one offset step per `4` GB frames.

## Recommended GB State Machine

The current `death_animation` byte in `SpritePlayer.custom_data` is too small a model for the original logic. Replace it with an explicit death mode plus timer.

Suggested player-local states:

- `playerAlive`
- `playerDeathBagCrushed`
- `playerDeathBugBounce`
- `playerDeathPreGraveWait`
- `playerDeathGraveFrames`
- `playerDeathFinalHold`

Suggested global round behavior:

- Set `isDying` when death starts, not after the sprite disappears.
- While `isDying` is active, freeze player control, enemy AI, bag updates, spawning, and level progression.
- Let only the active death animation and death audio continue.
- Trigger `resetLevelState()` only after the final hold completes.

This is closer to the original than the current `isDying = 128` shortcut.

## Suggested Sprite Contract

Current player sprite usage already consumes frames `0..23` for walk and discharged directions, and `24` for the current dead frame.

Recommended extension:

- `24`: crushed pose
- `25`: grave5
- `26`: grave4
- `27`: grave3
- `28`: grave2
- `29`: grave1

This matches the original logic, where the crushed pose is distinct from the grave sequence.

Important note:

- The remaster asset names are `diggerd` plus `grave5..grave1`, which strongly supports this layout.
- If your sheet prefers the reverse grave order in art, keep the code-facing order above and remap in the animation table.

## Animation Behavior To Implement

### Bag death on GB

1. Start death mode with cause `bag`.
2. Lock gameplay.
3. Keep player `x` fixed.
4. Clamp player `y` under the bag until the bag finishes falling.
5. Show crushed pose during this phase.
6. Start death sweep SFX when the bag stops.
7. Wait `20` GB frames.
8. Move player up `6` pixels once.
9. Show grave frames `25, 26, 27, 28, 29`.
10. Hold each grave frame for `8` GB frames.
11. Start the death tune when frame `25` is shown.
12. Hold the final grave frame for `240` GB frames.
13. Respawn or end the game.

### Bug death on GB

1. Start death mode with cause `bug`.
2. Lock gameplay.
3. Show crushed pose during the whole bounce.
4. Apply y offsets `-3, -5, -6, -6, -5, -3, 0` relative to the resting position.
5. Advance one offset every `4` GB frames.
6. Start death sweep SFX on the first bounce step.
7. After the bounce, wait `20` GB frames.
8. Reuse the same grave phase as bag death.

## Audio Breakdown

The original death audio has two layers:

1. A short death sweep sound effect
2. A separate death dirge melody

### Death sweep SFX

Original logic:

- Start at period `20000`
- For the first `10` updates, decrease by `1000` each step
- After that, increase by `500` each step
- Stop when the value exceeds `30000`

Reference:

- `snd_dead()` / `chk_dead()` in `/Users/andreabogazzi/develop/zgb-digger/digger_pc_src/original/Work/TH.C`
- `soundddie()` / `soundddieupdate()` in `/Users/andreabogazzi/develop/zgb-digger/digger_pc_src/remaster/sound.c`

For the GB port, the exact PC timer periods do not translate directly to GB registers, so the practical target is:

- a descending sweep for about `10` update steps
- then a shorter rising tail
- started at bag-stop for bag death
- started on bounce-step `0` for bug death

This can be implemented as:

- a dedicated SFX routine with a small period table
- or a hand-authored sequence in the existing ZGB sound system

### Death tune

Original note sequence:

- `REST`
- `C4`
- `C4`
- `C4`
- `C4`
- `D#4`
- `D4`
- `D4`
- `C4`
- `C4`
- `B3`
- `C4`

Original relative durations:

- `2, 6, 4, 2, 6, 4, 2, 4, 2, 4, 2, 12`

Then the tune appends long rests.

Reference:

- `music2[]` in `/Users/andreabogazzi/develop/zgb-digger/digger_pc_src/original/Work/TH.C`
- `dirge[]` in `/Users/andreabogazzi/develop/zgb-digger/digger_pc_src/remaster/sound.c`

Start the death tune on the first grave frame, not at death impact. That matches the original.

## Music Envelope Notes

The original three tunes use different envelopes.

Death tune envelope:

- max volume `50`
- attack rate `50`
- sustain level `25`
- decay rate `5`
- release rate `1`

Reference:

- `init_music(2)` in `/Users/andreabogazzi/develop/zgb-digger/digger_pc_src/original/Work/TH.C`
- `music(2)` in `/Users/andreabogazzi/develop/zgb-digger/digger_pc_src/remaster/sound.c`

For GB, preserving the exact ADSR is less important than preserving:

- note order
- relative note lengths
- the fact that the dirge is slower and more sustained than normal movement music

## Suggested Code Changes

### Player code

Update `/Users/andreabogazzi/develop/zgb-digger/ZGB-template-master/src/SpritePlayer.c` to:

- distinguish bag death from bug death
- replace the current one-byte countdown with a proper death mode and timer
- drive grave frames explicitly
- start the death tune at grave start
- remove the debug `J_B` death shortcut once the feature is in place

### Shared definitions

Update `/Users/andreabogazzi/develop/zgb-digger/ZGB-template-master/include/SpritePlayer.h` to reserve custom data slots for:

- death mode
- death timer
- death cause
- death frame index or bounce step

### Game state

Update `/Users/andreabogazzi/develop/zgb-digger/ZGB-template-master/src/StateGame.c` to:

- keep round-freeze logic active for the full death sequence
- defer respawn until the final grave hold ends
- stop normal level music while death audio is active

### Music / SFX assets

Add:

- one dedicated death tune asset
- one dedicated death sweep SFX asset, or a small code-driven sweep sequence

## Minimal Acceptance Target

Issue 25 is in good shape if the first pass delivers all of this:

1. Bag death uses crushed pose, pause, grave frames, final hold.
2. Bug death uses bounce arc, then the same grave sequence.
3. Death tune starts on the first grave frame.
4. A dedicated death sweep plays before the dirge.
5. Gameplay is frozen for the whole death sequence.
6. Respawn only happens after the sequence finishes.

## Nice-To-Have After First Pass

- remove monsters overlapped by the first grave frame, like the PC code does
- shorter final hold when music is disabled
- a more accurate sweep shape if the first SFX pass is only approximate
- optional palette or camera accent if it does not harm the original feel
