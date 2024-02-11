building tools for linux is very simple:
in the tools/gbr2c folder: g++ -o gbr2c gbr2c.cpp gbrParser.cpp 
in the tools/gbm2c folder: g++ -o gbm2c -I../gbr2c gbm2c.cpp ../gbr2c/gbrParser.cpp
in the tools/gbr2png folder: g++ -o gbr2png -I../gbr2c -I./lodepng gbr2png.cpp ./lodepng/lodepng.cpp ../gbr2c/gbrParser.cpp