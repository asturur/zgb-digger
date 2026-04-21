---
name: visual-regression-gbm
description: Generate the visual regression preview map for tunnel tiles by rewriting `assets_backup/visual_verification/visualRegression.gbm` from the `determineDigTiles()` switch cases in `ZGB-template-master/src/StateGame.c`. Use when the user wants to inspect tunnel tile quartets in Game Boy Map Builder.
---

# Visual Regression GBM

Use this skill when the user wants `determineDigTiles()` rendered into `assets_backup/visual_verification/visualRegression.gbm` for inspection in a map editor.

## Goal

Rewrite the tile payload of `assets_backup/visual_verification/visualRegression.gbm` so each `case` in `determineDigTiles()` becomes one 2x2 block laid out in switch order.

## Repo-Specific Facts

- `determineDigTiles()` lives in `ZGB-template-master/src/StateGame.c`.
- Tile numeric defines live in `ZGB-template-master/include/StateGame.h`.
- In this `.gbm` file, the tile payload starts at offset `0x0648`.
- Each map tile entry uses 3 bytes.
- The file is a `2 x 240` map, so each quartet is stacked vertically:
  - row 0: `tiles[0]`, `tiles[1]`
  - row 1: `tiles[2]`, `tiles[3]`
- The preview assets live together under `assets_backup/visual_verification/`.
- The preview map points at the local copy `assets_backup/visual_verification/commonTiles.gbr`.
- Preview indices use these rules:
  - `tileBlack` maps to preview tile `0`
  - `tileGrass` maps to preview tile `1`
  - other gameplay tiles use `define_value - 10`

## Workflow

1. Run `scripts/write_visual_regression_gbm.py`.
2. The script should:
   - parse tile defines from `StateGame.h`
   - parse `determineDigTiles()` cases from `StateGame.c`
   - start each case from default quartet `tileGrass, tileGrass, tileGrass, tileGrass`
   - apply explicit `tiles[n] = tileName;` assignments in each case
   - convert each tile name to a preview index using the special cases for `tileBlack` and `tileGrass`
   - rewrite the `.gbm` payload in switch order
   - zero-fill the unused tail of the map
3. Report how many cases were exported and any placeholder mapping that was applied.

## Validation

After writing the file:

- verify the file size did not change
- verify the tile payload still begins at `0x0648`
- verify the map is still `2 x 240`
- inspect the first few written entries if the user asks
- do not rebuild the ROM unless the user asks

## Notes

- Prefer the bundled script over ad-hoc shell edits.
- If the switch structure changes and the script cannot parse it safely, stop and fix the parser instead of patching the binary manually.
