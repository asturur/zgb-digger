---
name: gb-sameboy-runner
description: Use when the user wants to build the Game Boy ROM for this repository and launch it in SameBoy, especially for the ZGB Digger project where the build path, ROM output path, and emulator command should stay consistent.
---

# GB SameBoy Runner

Use this skill when the user asks to build the ROM and run it in SameBoy.

## Workflow

1. Run the build first and wait for it to finish; stop if the build fails.
2. Only after the build command exits successfully, launch SameBoy with the generated ROM using a single command.
3. Report the exact command used and whether it exited cleanly.

## Digger Defaults

- Build directory: `~/develop/zgb-digger/ZGB-template-master/src`
- Build command: `make gb`
- ROM path: `~/develop/zgb-digger/ZGB-template-master/bin/gb/Digger.gb`
- SameBoy app: `/Applications/SameBoy.app`
- Launch command:

```bash
open -a /Applications/SameBoy.app ~/develop/zgb-digger/ZGB-template-master/bin/gb/Digger.gb
```

## Notes

- Prefer the single `open -a ... <rom>` command when SameBoy is installed in `/Applications`.
- If SameBoy is not found, search for `SameBoy.app` before asking the user.
- Never start `make gb` and the SameBoy launch in parallel or in the same compound command.
- Wait for the final link step to complete before launching, otherwise SameBoy may open a stale ROM from the previous build.
- Do not claim the ROM was launched unless the build succeeded and the launch command exited with code `0`.
