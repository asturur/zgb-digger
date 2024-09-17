#include <gb/gb.h> 


// scores
#define scoreEmerald 25
#define scoreGold 500
#define scoreKill 250
#define maxLives 5

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

// tiles and metatiles codes
#define tileBlack 0
#define tileGrass 1
#define tileEmeraldTL 2
#define tileEmeraldTR 3
#define tileEmeraldBL 4
#define tileEmeraldBR 5
#define tileBagTL 6
#define tileBagTR 7
#define tileBagBL 8
#define tileBagBR 9

#define metaTileEmerald 16
#define metaTileBag 32
#define metaTileGold 64
#define EM metaTileEmerald
#define BG metaTileBag
#define mapMetaWidth 15
#define mapMetaHeight 10
#define metaTileGallery 0

// game timers and behaviour
#define maxEnimesCount 3
#define enemySpawnTimer 300

// useful macros
#define TILE_FROM_PIXEL(X) X >> tileSizeBitShift
#define MOD_FOR_TILE(X) X & 0x07

#define LARGE_TILE_FROM_PIXEL(X) X >> largetTileSizeBitShift
#define MOD_FOR_LARGE_TILE(X) X & 0x0F
// utility functions
void updateScore(uint16_t addScore);
BOOLEAN checkTilesFor(UBYTE column, UBYTE row, UBYTE type);
void addOnMap(uint16_t x, uint16_t y, uint8_t metaTile);