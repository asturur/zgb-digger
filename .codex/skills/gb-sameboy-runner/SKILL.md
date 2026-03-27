---
name: gb-sameboy-runner
description: Use when the user wants to build the Game Boy ROM for this repository and run or debug it with Emulicious, especially for the ZGB Digger project where the build path, ROM output path, emulator command, and VS Code debug setup should stay consistent.
---

# GB Emulicious Runner

Use this skill when the user asks to build the ROM and run or debug it in Emulicious.

## Workflow

1. Run the build first and wait for it to finish; stop if the build fails.
2. For a normal run, only after the build command exits successfully, launch Emulicious with the generated ROM.
3. For debugging, build with `BUILD_TYPE=Debug` and use the root `.vscode/launch.json` and `.vscode/tasks.json` in this repo.
4. Report the exact command used and whether it exited cleanly.

## Digger Defaults

- Build directory: `~/develop/zgb-digger/ZGB-template-master/src`
- Build command: `make gb`
- Debug build command: `make BUILD_TYPE=Debug gb`
- ROM path: `~/develop/zgb-digger/ZGB-template-master/bin/gb/Digger.gb`
- Debug ROM path: `~/develop/zgb-digger/ZGB-template-master/bin/gb/Digger_Debug.gb`
- Debug symbols: `~/develop/zgb-digger/ZGB-template-master/bin/gb/Digger_Debug.cdb`
- Emulicious path: `~/develop/zgb-digger/Emulicious/Emulicious.jar`
- Normal launch command:

```bash
java -jar ~/develop/zgb-digger/Emulicious/Emulicious.jar ~/develop/zgb-digger/ZGB-template-master/bin/gb/Digger.gb
```

- VS Code debug files:
  - `~/develop/zgb-digger/.vscode/tasks.json`
  - `~/develop/zgb-digger/.vscode/launch.json`

## Notes

- Prefer Emulicious over SameBoy for this repository from now on.
- Never start the build and the Emulicious launch in parallel or in the same compound command.
- Wait for the final link step to complete before launching, otherwise Emulicious may open a stale ROM from the previous build.
- The old `ZGB-template-master/.vscode` debug files are template leftovers and do not match this repository's actual output paths.
- The working debug entry point is the repo-root VS Code config in `~/.vscode` under this repository:
  - `~/develop/zgb-digger/.vscode/tasks.json`
  - `~/develop/zgb-digger/.vscode/launch.json`
- The `system` field should be omitted from the Emulicious launch configuration for this project. The extension rejected `"gb"` with `No enum constant ... EmulationSystem.GB`.
- Do not claim the ROM was launched unless the build succeeded and the launch command exited cleanly.
