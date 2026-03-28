# ZGB-template
A Digger project built on [CrossZGB](https://github.com/gbdk-2020/CrossZGB), the multi-platform successor to the original ZGB engine.

## Building this repo

This project vendors `CrossZGB-2026.1` and defaults `ZGB_PATH` to `CrossZGB-2026.1/ZGB/common`.

The vendored release already includes a matching `gbdk-2020` toolchain at `CrossZGB-2026.1/gbdk`, so no extra install is needed unless you want to override it via `GBDK_HOME`.

Then build from `ZGB-template-master/src` with:

```sh
make gb
```

The ROM is generated at `ZGB-template-master/bin/gb/Digger.gb`.

This repository currently stays Game Boy-only, so the existing flat asset layout under `res/` and `res/sprites/` remains valid with CrossZGB. If you later add platform-specific assets, place them under `res/backgrounds/gb`, `res/fonts/gb`, `res/music/gb`, `res/sounds/gb`, or `res/sprites/gb`.

building tools for linux is very simple:
in the tools/gbr2c folder: g++ -o gbr2c gbr2c.cpp gbrParser.cpp 
in the tools/gbm2c folder: g++ -o gbm2c -I../gbr2c gbm2c.cpp ../gbr2c/gbrParser.cpp
in the tools/gbr2png folder: g++ -o gbr2png -I../gbr2c -I./lodepng gbr2png.cpp ./lodepng/lodepng.cpp ../gbr2c/gbrParser.cpp


wine ~/develop/zgb-digger/CrossZGB-2026.1/env/tools/gbtd/gbtd.exe
wine ~/develop/zgb-digger/CrossZGB-2026.1/env/tools/gbmb/gbmb.exe
