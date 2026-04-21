#!/usr/bin/env python3

from __future__ import annotations

import re
import struct
from pathlib import Path


EXPECTED_MAP_WIDTH = 2
EXPECTED_MAP_HEIGHT = 240
EXPECTED_TILE_PAYLOAD_OFFSET = 0x0648
EXPECTED_TILESET_NAME = "commonTiles.gbr"


def repo_root() -> Path:
    return Path(__file__).resolve().parents[4]


def load_numeric_defines(header_path: Path) -> dict[str, int]:
    defines: dict[str, int] = {}
    define_re = re.compile(r"#define\s+(\w+)\s+(0x[0-9A-Fa-f]+|\d+)")

    for line in header_path.read_text().splitlines():
        match = define_re.match(line)
        if match:
            defines[match.group(1)] = int(match.group(2), 0)

    return defines


def extract_determine_dig_tiles_cases(source_path: Path) -> list[tuple[str, str, str, str]]:
    lines = source_path.read_text().splitlines()

    function_start = next(
        i for i, line in enumerate(lines) if line.startswith("void determineDigTiles(")
    )
    switch_start = next(
        i for i, line in enumerate(lines[function_start:], function_start) if "switch (currentCell)" in line
    )
    default_index = next(
        i for i, line in enumerate(lines[switch_start:], switch_start) if re.match(r"\s*default:\s*$", line)
    )

    tile_assign_re = re.compile(r"tiles\[(\d)\]\s*=\s*(\w+)\s*;")
    case_re = re.compile(r"\s*case\b")

    quartets: list[tuple[str, str, str, str]] = []
    in_case = False
    current = ["tileGrass", "tileGrass", "tileGrass", "tileGrass"]

    for line in lines[switch_start + 1 : default_index]:
        if case_re.match(line):
            if in_case:
                raise RuntimeError(f"Found a new case before break: {line.strip()}")
            current = ["tileGrass", "tileGrass", "tileGrass", "tileGrass"]
            in_case = True
            continue

        if not in_case:
            continue

        assign_match = tile_assign_re.search(line)
        if assign_match:
            current[int(assign_match.group(1))] = assign_match.group(2)

        if "break;" in line:
            quartets.append(tuple(current))
            in_case = False

    if in_case:
        raise RuntimeError("Unterminated case while parsing determineDigTiles()")

    return quartets


def locate_map_tile_payload(gbm_bytes: bytes) -> tuple[int, int, int, int]:
    pos = 4
    map_width = None
    map_height = None
    tile_file = None

    while pos + 20 <= len(gbm_bytes):
        object_type = struct.unpack_from("<H", gbm_bytes, pos + 6)[0]
        object_length = struct.unpack_from("<I", gbm_bytes, pos + 16)[0]
        payload_offset = pos + 20

        if object_type == 0x2:
            map_width, map_height = struct.unpack_from("<II", gbm_bytes, payload_offset + 128)
            tile_file_bytes = gbm_bytes[payload_offset + 140 : payload_offset + 396]
            tile_file = tile_file_bytes.split(b"\0", 1)[0].decode("latin1")
        elif object_type == 0x3:
            return payload_offset, object_length, map_width, map_height, tile_file

        pos = payload_offset + object_length

    raise RuntimeError("Could not find GBM tile payload")


def preview_tile_index(tile_name: str, defines: dict[str, int]) -> int:
    if tile_name == "tileBlack":
        return 0
    if tile_name == "tileGrass":
        return 1

    tile_value = defines[tile_name]
    if tile_value < 10:
        raise RuntimeError(f"Tile {tile_name} with value {tile_value} has no preview-map rule")
    return tile_value - 10


def write_preview_map() -> None:
    root = repo_root()
    header_path = root / "ZGB-template-master/include/StateGame.h"
    source_path = root / "ZGB-template-master/src/StateGame.c"
    preview_dir = root / "assets_backup/visual_verification"
    gbm_path = preview_dir / "visualRegression.gbm"

    original_bytes = gbm_path.read_bytes()
    gbm_data = bytearray(original_bytes)

    defines = load_numeric_defines(header_path)
    quartets = extract_determine_dig_tiles_cases(source_path)
    payload_offset, payload_length, map_width, map_height, tile_file = locate_map_tile_payload(gbm_data)

    if payload_offset != EXPECTED_TILE_PAYLOAD_OFFSET:
        raise RuntimeError(
            f"Unexpected GBM tile payload offset: 0x{payload_offset:04X}, expected 0x{EXPECTED_TILE_PAYLOAD_OFFSET:04X}"
        )

    if (map_width, map_height) != (EXPECTED_MAP_WIDTH, EXPECTED_MAP_HEIGHT):
        raise RuntimeError(
            f"Unexpected GBM map size: {map_width}x{map_height}, expected {EXPECTED_MAP_WIDTH}x{EXPECTED_MAP_HEIGHT}"
        )

    if tile_file != EXPECTED_TILESET_NAME:
        raise RuntimeError(
            f"Unexpected GBM tileset reference: {tile_file!r}, expected {EXPECTED_TILESET_NAME!r}"
        )

    tileset_path = preview_dir / EXPECTED_TILESET_NAME
    if not tileset_path.exists():
        raise RuntimeError(f"Missing preview tileset copy: {tileset_path}")

    capacity = payload_length // 3
    required_entries = len(quartets) * 4

    if required_entries > capacity:
        raise RuntimeError(
            f"Not enough GBM tile slots: need {required_entries}, have {capacity}"
        )

    entries: list[int] = []
    for quartet in quartets:
        entries.extend(preview_tile_index(tile_name, defines) for tile_name in quartet)

    entries.extend([0] * (capacity - len(entries)))

    for index, tile_index in enumerate(entries):
        entry_offset = payload_offset + index * 3
        gbm_data[entry_offset : entry_offset + 3] = bytes((0x00, 0x00, tile_index & 0xFF))

    if len(gbm_data) != len(original_bytes):
        raise RuntimeError("GBM size changed unexpectedly")

    gbm_path.write_bytes(gbm_data)

    print(f"Wrote {len(quartets)} quartets ({required_entries} tile entries) to {gbm_path}")
    print(f"Verified local preview tileset: {tileset_path}")
    print("Preview mapping rules: tileBlack -> 0, tileGrass -> 1, all other gameplay tiles -> define minus 10")
    print("First 8 quartets:")
    for index, quartet in enumerate(quartets[:8]):
        preview = [preview_tile_index(tile_name, defines) for tile_name in quartet]
        print(f"{index:02d}: {quartet} -> {preview}")


if __name__ == "__main__":
    write_preview_map()
