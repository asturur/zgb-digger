#include <gb/gb.h> 

// scores
#define scoreEmerald 25
#define scoreKill 250

// maps and tiles
#define tilesPerRow 32
#define scoreFontOffset 1
#define hudSize 64 // tilesPerRow * 2
#define lifeFont 11
#define tileSize 8
#define tileSizeBitShift 3

// map boundaries
#define mapBoundUp 24
#define mapBoundLeft 8
#define mapBoundRight 232
#define mapBoundDown 168

// game timers and behaviour
#define maxEnimesCount 3
#define enemySpawnTimer 300

void updateScore(uint16_t addScore);
