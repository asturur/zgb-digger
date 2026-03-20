# ZGB-template
A template for projects using [ZGB](https://github.com/Zal0/ZGB), A little engine for creating games for the original GameBoy

## Building this repo

This project vendors a `ZGB-2023.0` snapshot and defaults `ZGB_PATH` to that copy.

You still need a `gbdk-2020` toolchain installed. The current verified setup is `gbdk-2020` 4.2.0, either:

- unpacked at `ZGB-2023.0/env/gbdk`
- or exposed through `GBDK_HOME`

Then build from `ZGB-template-master/src` with:

```sh
make gb
```

The ROM is generated at `ZGB-template-master/bin/gb/Digger.gb`.

building tools for linux is very simple:
in the tools/gbr2c folder: g++ -o gbr2c gbr2c.cpp gbrParser.cpp 
in the tools/gbm2c folder: g++ -o gbm2c -I../gbr2c gbm2c.cpp ../gbr2c/gbrParser.cpp
in the tools/gbr2png folder: g++ -o gbr2png -I../gbr2c -I./lodepng gbr2png.cpp ./lodepng/lodepng.cpp ../gbr2c/gbrParser.cpp


wine ~/develop/zgb-digger/ZGB-2023.0/tools/gbtd22/GBTD.EXE
wine ~/develop/zgb-digger/ZGB-2023.0/tools/gbmb18/GBMB.EXE
