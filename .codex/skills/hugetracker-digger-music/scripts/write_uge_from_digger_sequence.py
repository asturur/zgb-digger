#!/usr/bin/env python3
"""Write hUGETracker v6 .uge files from Digger NOTE:DURATION streams.

The script intentionally keeps the writer small and deterministic. It supports a
linear reference file and an optional repeat-plan file. Repeat plans are
validated against the linear audible timeline using hUGEDriver's Dxx behavior:
encoded D param = target row + 1.
"""

from __future__ import annotations

import argparse
import json
import re
import struct
import sys
from dataclasses import dataclass
from pathlib import Path

NO_NOTE = 90
PATTERN_ROWS = 64
CELL_BYTES = 17
INSTRUMENT_COUNT = 45
INSTRUMENT_BYTES = 1385


NOTE_NAMES = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]
NOTE_CODES = {
    (f"{name}-{octave}" if len(name) == 1 else f"{name}{octave}"): (octave - 3) * 12 + semi
    for octave in range(3, 9)
    for semi, name in enumerate(NOTE_NAMES)
}

DEFAULT_WAVES = [
    [0, 0, 0, 0, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15],
    [0, 0, 0, 0, 0, 0, 0, 0, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 15, 15, 15, 15, 15, 15, 15],
    [0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15],
    [15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15],
    [7, 10, 12, 13, 13, 11, 7, 5, 2, 1, 1, 3, 6, 8, 11, 13, 13, 12, 9, 7, 4, 1, 0, 1, 4, 7, 9, 12, 13, 13, 11, 8],
    [0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15],
    [15, 14, 15, 12, 15, 10, 15, 8, 15, 6, 15, 4, 15, 2, 15, 0, 15, 2, 15, 4, 15, 6, 15, 8, 15, 10, 15, 12, 15, 14, 15, 15],
    [15, 14, 13, 13, 12, 12, 11, 11, 10, 10, 9, 9, 8, 8, 7, 7, 8, 10, 11, 13, 15, 1, 2, 4, 5, 7, 8, 10, 11, 13, 14, 14],
    [8, 4, 1, 1, 6, 1, 14, 13, 5, 7, 4, 7, 5, 10, 10, 13, 12, 14, 10, 3, 1, 7, 7, 9, 13, 13, 2, 0, 0, 3, 4, 7],
]


@dataclass(frozen=True)
class Event:
    token: str
    note: int | None
    duration: int


@dataclass
class Cell:
    note: int = NO_NOTE
    instrument: int = 0
    volume: int = 0
    effect_code: int = 0
    effect_param: int = 0


def i32(value: int) -> bytes:
    return struct.pack("<i", value)


def pascal_string(value: str) -> bytes:
    raw = value.encode("ascii", errors="replace")[:255]
    return bytes([len(raw)]) + raw + bytes(255 - len(raw))


def ansi_string(value: str) -> bytes:
    raw = value.encode("utf-8")
    return i32(len(raw)) + raw


def blank_cell() -> Cell:
    return Cell()


def pack_cell(cell: Cell) -> bytes:
    return b"".join(
        [
            i32(cell.note),
            i32(cell.instrument),
            i32(cell.volume),
            i32(cell.effect_code),
            bytes([cell.effect_param & 0xFF]),
        ]
    )


def blank_pattern() -> list[Cell]:
    return [blank_cell() for _ in range(PATTERN_ROWS)]


def pack_pattern(rows: list[Cell]) -> bytes:
    if len(rows) != PATTERN_ROWS:
        raise ValueError(f"pattern has {len(rows)} rows, expected {PATTERN_ROWS}")
    return b"".join(pack_cell(cell) for cell in rows)


def pack_instrument(
    *,
    type_: int,
    name: str = "",
    duty: int = 2,
    volume: int = 15,
    vol_sweep_amount: int = 0,
    waveform: int = 0,
    counter_step: int = 0,
) -> bytes:
    data = b"".join(
        [
            i32(type_),
            pascal_string(name),
            i32(0),
            bytes([0]),
            bytes([0 if type_ == 1 else volume]),
            i32(1),
            bytes([vol_sweep_amount]),
            i32(0),
            i32(1),
            i32(0),
            bytes([duty]),
            i32(1),
            i32(waveform),
            i32(counter_step),
            bytes([0]),
            pack_pattern(blank_pattern()),
        ]
    )
    if len(data) != INSTRUMENT_BYTES:
        raise ValueError(f"instrument size is {len(data)}, expected {INSTRUMENT_BYTES}")
    return data


def default_instruments() -> list[bytes]:
    duty_names = {
        1: ("Duty 12.5%", 0, 0),
        2: ("Digger Duty 25%", 1, 0),
        3: ("Duty 50%", 2, 0),
        4: ("Duty 75%", 3, 0),
        5: ("Duty 12.5% plink", 0, 1),
        6: ("Duty 25% plink", 1, 1),
        7: ("Duty 50% plink", 2, 1),
        8: ("Duty 75% plink", 3, 1),
    }
    instruments: list[bytes] = []
    for idx in range(1, 16):
        name, duty, sweep = duty_names.get(idx, ("", 2, 0))
        instruments.append(pack_instrument(type_=0, name=name, duty=duty, vol_sweep_amount=sweep))

    wave_names = [
        "Square wave 12.5%",
        "Square wave 25%",
        "Square wave 50%",
        "Square wave 75%",
        "Sawtooth wave",
        "Triangle wave",
        "Sine wave",
        "Toothy",
        "Triangle Toothy",
        "Pointy",
        "Strange",
    ]
    for idx in range(1, 16):
        instruments.append(pack_instrument(type_=1, name=wave_names[idx - 1] if idx <= len(wave_names) else "", waveform=idx - 1))

    for _ in range(1, 16):
        instruments.append(pack_instrument(type_=2, counter_step=0))

    if len(instruments) != INSTRUMENT_COUNT:
        raise ValueError("wrong instrument count")
    return instruments


def parse_events(text: str) -> list[Event]:
    events: list[Event] = []
    for line in text.splitlines():
        line = re.sub(r"^\s*\d+:\s*", "", line).strip()
        if not line:
            continue
        for token in line.split():
            match = re.fullmatch(r"((?:[A-G](?:#|-)[3-8])|---):(\d{2})", token)
            if not match:
                raise ValueError(f"bad token: {token}")
            note_name, duration_text = match.groups()
            if note_name == "---":
                events.append(Event(token=token, note=None, duration=int(duration_text)))
                continue
            if note_name not in NOTE_CODES:
                raise ValueError(f"unknown note: {note_name}")
            events.append(Event(token=token, note=NOTE_CODES[note_name], duration=int(duration_text)))
    return events


def rows_for_event(event: Event, instrument: int, duration_divisor: int, release_rows: int) -> list[Cell]:
    if event.duration % duration_divisor != 0:
        raise ValueError(f"duration is not divisible by {duration_divisor}: {event.token}")
    total_rows = event.duration // duration_divisor
    if total_rows <= 0:
        raise ValueError(f"duration maps to no rows: {event.token}")

    if event.note is None:
        rows = [blank_cell() for _ in range(total_rows)]
        rows[0] = Cell(effect_code=0xE, effect_param=0x00)
        return rows

    rows = [blank_cell() for _ in range(total_rows)]
    rows[0] = Cell(note=event.note, instrument=instrument)
    if total_rows > release_rows and release_rows > 0:
        for index in range(total_rows - release_rows, total_rows):
            rows[index] = Cell(effect_code=0xC, effect_param=0x04)
    return rows


def event_rows(events: list[Event], instrument: int, duration_divisor: int, release_rows: int) -> list[Cell]:
    rows: list[Cell] = []
    for event in events:
        rows.extend(rows_for_event(event, instrument, duration_divisor, release_rows))
    return rows


def chunk_linear_patterns(rows: list[Cell]) -> list[list[Cell]]:
    count = (len(rows) + PATTERN_ROWS - 1) // PATTERN_ROWS
    patterns: list[list[Cell]] = []
    for idx in range(count):
        chunk = rows[idx * PATTERN_ROWS : (idx + 1) * PATTERN_ROWS]
        patterns.append(chunk + [blank_cell() for _ in range(PATTERN_ROWS - len(chunk))])
    return patterns


def make_repeat_patterns(
    events: list[Event],
    plan: dict,
    instrument: int,
    duration_divisor: int,
    release_rows: int,
) -> tuple[list[list[Cell]], list[int], list[dict]]:
    patterns: list[list[Cell]] = []
    break_report: list[dict] = []
    for index, segment in enumerate(plan["segments"]):
        start = int(segment["start"])
        end = int(segment["end"])
        start_row = int(segment.get("start_row", 0))
        rows = blank_pattern()
        segment_rows = event_rows(events[start:end], instrument, duration_divisor, release_rows)
        if start_row < 0 or start_row + len(segment_rows) > PATTERN_ROWS:
            raise ValueError(f"segment {index} does not fit in one pattern")
        for offset, cell in enumerate(segment_rows):
            rows[start_row + offset] = cell
        if segment.get("break_to_row") is not None:
            end_row = start_row + len(segment_rows) - 1
            if rows[end_row].effect_code or rows[end_row].effect_param:
                raise ValueError(f"segment {index} break conflicts with existing effect at row {end_row}")
            target = int(segment["break_to_row"])
            rows[end_row].effect_code = 0xD
            rows[end_row].effect_param = target + 1
            break_report.append(
                {
                    "pattern": index,
                    "row": end_row,
                    "encoded_param": target + 1,
                    "effective_next_row": target,
                }
            )
        patterns.append(rows)
    order = [int(value) for value in plan["order"]]
    return patterns, order, break_report


def simulate(patterns: list[list[Cell]], order: list[int]) -> list[Cell]:
    out: list[Cell] = []
    order_index = 0
    row = 0
    max_rows = 10000
    while order_index < len(order) - 1 and len(out) < max_rows:
        pattern_index = order[order_index]
        cell = patterns[pattern_index][row]
        out.append(cell)
        if cell.effect_code == 0xD and cell.effect_param != 0:
            order_index += 1
            row = cell.effect_param - 1
        else:
            row += 1
            if row >= PATTERN_ROWS:
                order_index += 1
                row = 0
    return out


def audible(cell: Cell) -> tuple[int, int, int, int, int]:
    if cell.effect_code == 0xD:
        return (cell.note, cell.instrument, cell.volume, 0, 0)
    return (cell.note, cell.instrument, cell.volume, cell.effect_code, cell.effect_param)


def validate_repeat(linear_patterns: list[list[Cell]], repeat_patterns: list[list[Cell]], repeat_order: list[int]) -> None:
    linear_order = list(range(len(linear_patterns))) + [0]
    linear_rows = simulate(linear_patterns, linear_order)
    repeat_rows = simulate(repeat_patterns, repeat_order)
    if len(linear_rows) != len(repeat_rows):
        raise ValueError(f"repeat playback length {len(repeat_rows)} != linear length {len(linear_rows)}")
    for index, (left, right) in enumerate(zip(linear_rows, repeat_rows)):
        if audible(left) != audible(right):
            raise ValueError(f"repeat mismatch at playback row {index}: {audible(left)} != {audible(right)}")


def write_uge(
    *,
    output: Path,
    title: str,
    comment: str,
    ticks_per_row: int,
    patterns: list[list[Cell]],
    duty1_order: list[int],
    silent_pattern_key: int,
) -> None:
    pattern_records = [(idx, pack_pattern(pattern)) for idx, pattern in enumerate(patterns)]
    pattern_records.append((silent_pattern_key, pack_pattern(blank_pattern())))
    silent_order = [silent_pattern_key for _ in range(len(duty1_order) - 1)] + [0]

    def order_matrix(order: list[int]) -> bytes:
        return i32(len(order)) + b"".join(i32(value) for value in order)

    waves = [bytes(DEFAULT_WAVES[idx]) if idx < len(DEFAULT_WAVES) else bytes(32) for idx in range(16)]
    data = b"".join(
        [
            i32(6),
            pascal_string(title),
            pascal_string("Codex"),
            pascal_string(comment),
            b"".join(default_instruments()),
            b"".join(waves),
            i32(ticks_per_row),
            bytes([0]),
            i32(0),
            i32(len(pattern_records)),
            b"".join(i32(key) + packed for key, packed in pattern_records),
            order_matrix(duty1_order),
            order_matrix(silent_order),
            order_matrix(silent_order),
            order_matrix(silent_order),
            b"".join(ansi_string("") for _ in range(16)),
        ]
    )
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_bytes(data)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, help="Text file containing NOTE:DURATION rows. Reads stdin if omitted.")
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--title", default="Digger Sequence")
    parser.add_argument("--comment", default="Generated from Digger NOTE:DURATION stream.")
    parser.add_argument("--ticks-per-row", type=int, default=12)
    parser.add_argument("--instrument", type=int, default=2)
    parser.add_argument("--duration-divisor", type=int, default=2, help="Map duration to rows with rows = duration / divisor.")
    parser.add_argument("--release-rows", type=int, default=1, help="Number of tail rows at the end of non-rest notes.")
    parser.add_argument("--repeat-plan", type=Path, help="JSON repeat plan with segments and order.")
    args = parser.parse_args()

    text = args.input.read_text() if args.input else sys.stdin.read()
    events = parse_events(text)
    linear_rows = event_rows(events, args.instrument, args.duration_divisor, args.release_rows)
    linear_patterns = chunk_linear_patterns(linear_rows)

    if args.repeat_plan:
        plan = json.loads(args.repeat_plan.read_text())
        repeat_patterns, order, breaks = make_repeat_patterns(
            events,
            plan,
            args.instrument,
            args.duration_divisor,
            args.release_rows,
        )
        validate_repeat(linear_patterns, repeat_patterns, order)
        patterns = repeat_patterns
        duty1_order = order
        mode = "repeat"
    else:
        patterns = linear_patterns
        duty1_order = list(range(len(patterns))) + [0]
        breaks = []
        mode = "linear"

    write_uge(
        output=args.output,
        title=args.title,
        comment=args.comment,
        ticks_per_row=args.ticks_per_row,
        patterns=patterns,
        duty1_order=duty1_order,
        silent_pattern_key=len(patterns),
    )

    print(
        json.dumps(
            {
                "output": str(args.output),
                "mode": mode,
                "events": len(events),
                "linear_rows": len(linear_rows),
                "patterns": len(patterns) + 1,
                "duty1_order": duty1_order,
                "ticks_per_row": args.ticks_per_row,
                "duration_divisor": args.duration_divisor,
                "release_rows": args.release_rows,
                "breaks": breaks,
            },
            indent=2,
        )
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
