#include <gb/gb.h> 


// scores
#define scoreEmerald 25
#define scoreKill 250

// maps and tiles
#define tilesPerRow 32
#define scoreFontOffset 1
#define hudSize 32 // tilesPerRow * 2
#define lifeFont 11
#define tileSize 8
#define largeTileSize 16
#define tileSizeBitShift 3
#define largetTileSizeBitShift 4

// map boundaries in pixels
#define mapBoundUp 16
#define mapBoundLeft 8
#define mapBoundRight 232
#define mapBoundDown 160

// game timers and behaviour
#define maxEnimesCount 3
#define enemySpawnTimer 300

// useful macros
#define TILE_FROM_PIXEL(X) X >> tileSizeBitShift
#define MOD_FOR_TILE(X) X & 0x07

#define LARGE_TILE_FROM_PIXEL(X) X >> largetTileSizeBitShift
#define MOD_FOR_LARGE_TILE(X) X & 0x15
// utility functions
void updateScore(uint16_t addScore);
