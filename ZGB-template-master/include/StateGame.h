#include <gb/gb.h> 
#include "Sprite.h"


// scores
#define scoreEmerald 25
#define scoreGold 500
#define scoreKill 250
#define maxLives 5

// maps and tiles
#define tilesPerRow 32
#define tilesPerColumn 23
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

// tiles for gold on black
#define goldTL 10
#define goldBL 11
#define goldTR 12
#define goldBR 13

// tiles for bag on black
#define bagTL 14
#define bagTR 15
#define bagBL 16
#define bagBR 17

#define tunnelMask 0x0F

#define metaTileEmerald 16
#define metaTileBag 32
#define metaTileGold 64
#define EM metaTileEmerald
#define BG metaTileBag
#define mapMetaWidth 15
#define mapMetaHeight 10
#define metaTileGallery 0

// game timers and behaviour
#define originalTickToGameBoyFrameRatio 4 // Keep the original 4:1 ratio in mind, but don't derive timings from it here.
#define enemyFirstSpawnTimer 40 // 10 original * 4
#define maxDifficultyLevel 10
#define totalEnemiesBaseCount 5
#define maxEnemiesOnScreenLevel1 3
#define maxEnemiesOnScreenLevel2To7 4
#define maxEnemiesOnScreenLevel8To10 5

// useful macros
#define TILE_FROM_PIXEL(X) ((X) >> tileSizeBitShift)
#define MOD_FOR_TILE(X) ((X) & 0x07)

#define LARGE_TILE_FROM_PIXEL(X) ((X) >> largetTileSizeBitShift)
#define MOD_FOR_LARGE_TILE(X) ((X) & 0x0F)
// utility functions
void updateScore(uint16_t addScore) BANKED;
BOOLEAN checkTilesFor(UBYTE column, UBYTE row, UBYTE type) NONBANKED;
UBYTE getTileMapTile(UBYTE column, UBYTE row) NONBANKED;
UBYTE getMapMetaTileArrayPosition(uint16_t x, uint16_t y) NONBANKED;
void addOnMap(uint16_t x, uint16_t y, uint8_t metaTile) NONBANKED;
void updateVideoMemAndMap(UBYTE column, UBYTE row, UBYTE type) NONBANKED;
void runMapSideEffects(void) BANKED;
Sprite* activateBag(uint8_t bagcell) BANKED;
UBYTE tryPushBagChainFromCell(UBYTE currentCell, UBYTE direction) BANKED;
void beginDeathFreeze(void) BANKED;
void playDeathMusic(void) BANKED;
void queueDeathRespawn(uint16_t frames) BANKED;

extern UBYTE currentLevel;
extern UBYTE difficultyLevel;
extern unsigned char levelMap[150];
extern uint8_t isDying;
extern BOOLEAN infiniteLives;
extern uint8_t lives;
extern BOOLEAN paused;