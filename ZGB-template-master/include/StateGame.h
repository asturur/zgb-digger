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

// tunnels with walls
#define tileLeftWall 18
#define tileRightWall 19
#define tileTopWall 20
#define tileBottomWall 21
#define tileTopLeftWall 22
#define tileTopRightWall 23
#define tileBottomRightWall 24
#define tileBottomLeftWall 25
#define tileHalfDigLeftTop 26
#define tileHalfDigRightTop 27
#define tileHalfDigTopLeft 28
#define tileHalfDigBottomLeft 29
#define tileDig75TopLeft 30
#define tileDig75TopRight 31
#define tileDig75BottomRight 32
#define tileDig75BottomLeft 33
#define tileHalfDigLeftBottom 34
#define tileHalfDigRightBottom 35
#define tileHalfDigTopRight 36
#define tileHalfDigBottomRight 37

#define tunnelHorizontalMask 0x0F
#define tunnelVerticalMask 0xF0

// PREFERRED TUNNEL BIT MASKS
#define tunnelHorizontalStep1 0x01
#define tunnelHorizontalStep12 0x03
#define tunnelHorizontalStep123 0x07
#define tunnelHorizontalStep234 0x0E
#define tunnelHorizontalStep34 0x0C
#define tunnelHorizontalStep4 0x08
#define tunnelHorizontalCenterMask 0x06

#define tunnelVerticalStep1 0x10
#define tunnelVerticalStep12 0x30
#define tunnelVerticalStep123 0x70
#define tunnelVerticalStep234 0xE0
#define tunnelVerticalStep34 0xC0
#define tunnelVerticalStep4 0x80
#define tunnelVerticalCenterMask 0x60

// END OF PREFERRED TUNNEL BIT MASKS
// Temporary compatibility aliases for older tunnel-mask names.
#define tunnelHorizontalLeftTileMask tunnelHorizontalStep12
#define tunnelHorizontalRightTileMask tunnelHorizontalStep34

#define tunnelVerticalTopTileMask tunnelVerticalStep12
#define tunnelVerticalBottomTileMask tunnelVerticalStep34

#define itemNone 0
#define itemEmerald 1
#define itemBag 2
#define itemGold 3
#define itemBonus 4

#define seedItemEmerald 16
#define seedItemBag 32
#define seedItemGold 64
#define legacyTunnelMask 0x0F
#define legacySeedTunnelLeft 0x01
#define legacySeedTunnelRight 0x02
#define legacySeedTunnelDown 0x04
#define legacySeedTunnelUp 0x08
#define EM seedItemEmerald
#define BG seedItemBag
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
UBYTE getMapMetaTileArrayPosition(uint16_t x, uint16_t y) NONBANKED;
void renderMetaCell(UBYTE cell) BANKED;
void determineDigTiles(
    UBYTE currentCell,
    UBYTE topCell,
    UBYTE rightCell,
    UBYTE bottomCell,
    UBYTE leftCell,
    UBYTE* tiles
) BANKED;
void openTunnelConnection(UBYTE fromCell, UBYTE direction) NONBANKED;
void updateVideoMemAndMap(UBYTE column, UBYTE row, UBYTE type) NONBANKED;
void runMapSideEffects(void) BANKED;
Sprite* activateBag(uint8_t bagcell) BANKED;
UBYTE tryPushBagChainFromCell(UBYTE currentCell, UBYTE direction) BANKED;
void beginDeathFreeze(void) BANKED;
void playDeathMusic(void) BANKED;
void queueDeathRespawn(uint16_t frames) BANKED;

extern UBYTE currentLevel;
extern UBYTE difficultyLevel;
extern unsigned char itemMap[150];
extern unsigned char tunnelMap[150];
extern uint8_t isDying;
extern BOOLEAN infiniteLives;
extern uint8_t lives;
extern BOOLEAN paused;
