---
name: hugetracker-digger-music
description: Create or revise hUGETracker .uge music files for the ZGB Digger project from Digger-style NOTE:DURATION text streams, including correct timing quantization, Game Boy channel choices, release tails, pattern reuse experiments, and hUGEDriver Dxx pattern-break validation.
---

# hUGETracker Digger Music

Use this skill when converting Digger note streams into `.uge` files, editing hUGETracker music assets, or testing pattern reuse for ROM size in this repository.

## Core Rules

- First confirm the token meaning. In Digger note streams, `NOTE:DURATION` means duration, not instrument.
- Preserve the audible timeline before optimizing pattern layout.
- Generate a linear known-good `.uge` first when the timing interpretation changes.
- Generate repeat-pattern variants only after there is a linear reference, and simulate the repeat variant against the linear timeline.
- Do not judge by `.uge` file size alone. hUGETracker exports full 64-row pattern data, while repeated order entries may reduce compiled ROM data only if the build/export path reuses pattern labels.

## Timing

For the current Digger timing pass:

- `:02` = one tracker row held for the full row.
- `:04` = two tracker rows: one held note row, then one release/delay row.
- `TicksPerRow = 12`, approximately `164.8 ms` per row at `72.8 Hz`.
- Use effect `C04` for the release/delay row unless the user asks for a different tail.
- Do not insert extra rests between notes unless a rest is explicitly present.

For older bonus-jingle timing, do not reuse these defaults blindly. Reconfirm whether each duration unit should map directly to rows, and whether the last row of every note slot should be a release tail.

For note streams with explicit rests:

- `---:NN` means a rest/silence slot.
- Generate a channel cut on the first row of the rest slot, then leave the remaining rest rows blank.
- If the timing table says `:02` has both hold and release, use `--duration-divisor 1 --release-rows 1` so `:02` becomes two rows.

## hUGEDriver Pattern Breaks

hUGEDriver stores `Dxx` pattern-break params as `target_row + 1`.

- To break to next pattern row `0`, encode param `1`.
- To break to next pattern row `20`, encode param `21`.
- Param `0` means no break in the driver path. Do not use `D00` to jump to row `0`.
- A row can only carry one effect. Do not end a repeat segment on a generated `C04` release row if that segment also needs a `Dxx` break; choose a segment boundary that ends on a held-note row or move the next segment start row instead.
- When describing this to the user, distinguish the encoded param from the effective target row.

## Workflow

1. Put the note stream in a text file or pass it on stdin.
2. Use `scripts/write_uge_from_digger_sequence.py` for deterministic `.uge` generation.
3. For a repeat variant, provide a repeat-plan JSON and let the script validate playback equivalence before writing.
4. Report output path, timing assumptions, pattern count, order table, and any pattern-break targets.

## Script Examples

Linear file:

```bash
python3 .codex/skills/hugetracker-digger-music/scripts/write_uge_from_digger_sequence.py \
  --input /tmp/sequence.txt \
  --output ZGB-template-master/res/music/digger_sequence.uge \
  --title "Digger Sequence" \
  --duration-divisor 2 \
  --release-rows 1
```

Repeat-plan file:

```bash
python3 .codex/skills/hugetracker-digger-music/scripts/write_uge_from_digger_sequence.py \
  --input /tmp/sequence.txt \
  --output ZGB-template-master/res/music/digger_sequence_repeats.uge \
  --title "Digger Sequence Repeats" \
  --duration-divisor 2 \
  --release-rows 1 \
  --repeat-plan /tmp/repeat_plan.json
```

Repeat-plan JSON shape:

```json
{
  "segments": [
    {"name": "intro_a", "start": 0, "end": 30, "start_row": 0, "break_to_row": 0},
    {"name": "repeat_a", "start": 60, "end": 76, "start_row": 0, "break_to_row": 20}
  ],
  "order": [0, 1, 1, 0]
}
```

Segment `start` and `end` are event indices, not row indices. `break_to_row` is the effective row number; the script encodes `target + 1` in the `.uge`.

## Validation

- Simulate rows with hUGEDriver semantics:
  - `Dxx` breaks only when encoded param is nonzero.
  - effective next row is `param - 1`.
  - order table's final sentinel is not played.
- Compare the repeat variant against the linear audible cells, ignoring `Dxx` itself but preserving notes, instruments, volume effects, and release effects.
- If simulation differs, do not deliver the repeat file as equivalent.
