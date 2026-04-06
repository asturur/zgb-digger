#include "Banks/SetAutoBank.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"
#include "MapInfo.h"
#include <gb/gb.h> 
#include <string.h>
#include <Sound.h>
#include "Sounds.h"
#include "Music.h"
#include "StateGame.h"
#include "SpritePlayer.h"

extern const UBYTE direction;
extern const UBYTE oppositeDirection;

BANKREF_EXTERN(Levels)
extern const unsigned char level1Map[150];
extern const unsigned char level2Map[150];
extern const unsigned char level3Map[150];
extern const unsigned char level4Map[150];
extern const unsigned char level5Map[150];
extern const unsigned char level6Map[150];
extern const unsigned char level7Map[150];
extern const unsigned char level8Map[150];
extern const unsigned char levelDebugMap[150];

extern uint8_t fx_00[];
extern void __mute_mask_fx_00;

// options
BOOLEAN infiniteLives = FALSE;
BOOLEAN invincibility = FALSE;
BOOLEAN debugMode = FALSE;

//
UBYTE currentLevel = 0;
UBYTE difficultyLevel = 0;
uint16_t score = 0;
UBYTE diamonds = 0;
uint8_t spawnTimer = 0;
uint8_t enemyCountOnScreen = 0;
uint8_t enemyMaxOnScreen = 0;
uint8_t enemyMaxTotal = 0;
uint8_t enemySpawned = 0;
uint8_t lives = 3;

uint8_t emeraldLoop = EMERALD_DING_QTY;
uint8_t emeraldDuration = EMERALD_DING_GAP_DURATION;
uint8_t emeraldScaleTimer = 0;
uint8_t emeraldFreq[] = { fxC_4, fxD_4, fxE_4, fxF_4, fxG_4, fxA_4, fxB_4, fxC_5 };
uint8_t emeraldFreqIndex = 0;

uint8_t lastVisitedMetaCell = 0;

// handles the death state.
// sprites do not move while the active death sequence continues
uint8_t isDying = 0;
static BOOLEAN deathRespawnQueued = FALSE;
static uint16_t deathRespawnTimer = 0;

DECLARE_MUSIC(popcorn);
DECLARE_MUSIC(dirge);

// currently loaded map
unsigned char levelMap[150];

struct MapInfo currentInMemoryLevel;
unsigned char tileMap[736];


UBYTE getTileMapTile(UBYTE column, UBYTE row) NONBANKED {
	// Use the RAM mirror for gameplay checks instead of reading live VRAM.
	if (column >= tilesPerRow || row >= tilesPerColumn) {
		return tileGrass;
	}
	return tileMap[row * tilesPerRow + column];
}

void updateVideoMemAndMap(UBYTE column, UBYTE row, UBYTE type) NONBANKED {
	set_bkg_tile_xy(column, row, type);
    tileMap[row * tilesPerRow + column] = type;
}

BOOLEAN checkTilesFor(UBYTE column, UBYTE row, UBYTE type) NONBANKED {
    return getTileMapTile(column, row) == type ||
        getTileMapTile(column + 1, row) == type || 
        getTileMapTile(column, row + 1) == type || 
        getTileMapTile(column + 1, row + 1) == type;
}

static void paintScore(void) {
	// scores are multiple of 25 points.
	// mod 2 = 1 draw a 5.
	// then mod 4 for 2/5/7
	// then >> 2 will give us the hundreds.
	// then we need to loop again
	uint8_t mod = 0;
	uint16_t lastScore = score;
	for (uint8_t i = 6; i > 0; i--) {
		mod = lastScore % 10;
		UPDATE_HUD_TILE(i, 0, scoreFontOffset + mod);
		lastScore = (lastScore - mod) / 10;
	}
	for (uint8_t i = 1; i <= maxLives; i++) {
		UPDATE_HUD_TILE(7 + i, 0, lives >= i ? lifeFont : 0);
	}
	UPDATE_HUD_TILE(14, 0, scoreFontOffset + enemyCountOnScreen);
}

void copyTileMapToRam(uint8_t levelToLoadBank, struct MapInfo *levelToLoad) BANKED {
	uint8_t __save = CURRENT_BANK;
	SWITCH_ROM(levelToLoadBank);
	// copy everything
	currentInMemoryLevel = *levelToLoad;
	// the data in the array is filled in by copyLevelMapToRam
	// redefine the pointer to my in memory array
	currentInMemoryLevel.data = tileMap;
	currentInMemoryLevel.width = 32;
	currentInMemoryLevel.height = 23;
	// if (levelToLoad->attributes) memcpy(map_attr, levelToLoad->attributes, current_level.width * current_level.height); else memset(map_attr, 0, sizeof(map_attr));
	SWITCH_ROM(__save);
}

void copyLevelMapToRam(unsigned char *mapToLoad[], uint8_t levelToLoadBank, struct MapInfo *levelToLoad) BANKED {
	// uint8_t __save = CURRENT_BANK;
	// SWITCH_ROM(mapToLoadBank);
	memcpy(levelMap, mapToLoad, 150);
	int8_t i, j;
	// fill up the first lines of the map with 2 lines 0s and 1 of 1s
	memset(tileMap, 0, hudSize);
	memset(tileMap + hudSize, 1, tilesPerRow * 22);
	uint8_t metaTile;
	// third row start at hudS, we skip the first tile
	uint16_t offset = hudSize + tilesPerRow + 1;

	// loop the map
	for (i = 0; i < mapMetaHeight; i++) {
		for (j = 0; j < mapMetaWidth; j++) {
			metaTile = levelMap[i * mapMetaWidth + j];
			if (metaTile > metaTileGallery && metaTile < metaTileEmerald) {
				// fill in four 0 tiles, the black gallery
				// at current row
				tileMap[offset] = tileBlack;
				tileMap[offset + 1] = tileBlack;
				// at next row
				tileMap[offset + tilesPerRow] = tileBlack;
				tileMap[offset + tilesPerRow + 1] = tileBlack;
			} else if (metaTile == metaTileEmerald) {
				tileMap[offset] = tileEmeraldTL;
				tileMap[offset + 1] = tileEmeraldTR;
				// at next row
				tileMap[offset + tilesPerRow] = tileEmeraldBL;
				tileMap[offset + tilesPerRow + 1] = tileEmeraldBR;
			} else if (metaTile == metaTileBag) {
				tileMap[offset] = tileBagTL;
				tileMap[offset + 1] = tileBagTR;
				// at next row
				tileMap[offset + tilesPerRow] = tileBagBL;
				tileMap[offset + tilesPerRow + 1] = tileBagBR;
			}
			offset += 2;
		}
		// skip last column the 31st and the 1st of next line
		// and skip a full line
		offset = offset + tilesPerRow + 2;
	}
	// SWITCH_ROM(__save);
	copyTileMapToRam(levelToLoadBank, levelToLoad);
	InitScroll(levelToLoadBank, &currentInMemoryLevel, 0, 0);
}

Sprite* activateBag(uint8_t bagcell) BANKED {
	// remove the bag tiles and replace with grass.
	// remove the bag from the map replace with grass
	// activate bag sprite
	uint8_t column = bagcell % mapMetaWidth;
	uint8_t row = (bagcell - column) / mapMetaWidth;
	uint8_t positionX = mapBoundLeft + column * 16;
	uint8_t positionY = mapBoundUp + row * 16;
	Sprite* bag = SpriteManagerAdd(SpriteBag, positionX, positionY);
	if (bag != 0) {
		levelMap[bagcell] -= metaTileBag;
	}
	return bag;
}


void updateScore(uint16_t addScore) {
	score += addScore;
	paintScore();
}

uint8_t getEnemySpawnGapTimer(void) {
	return enemySpawnGapBaseTimer - (difficultyLevel * enemySpawnGapDifficultyStep);
}

static void updateEmeraldSound(void) {
	if (emeraldScaleTimer > 0) {
		emeraldScaleTimer--;
	} else {
		emeraldFreqIndex = 0;
	}
	if (emeraldLoop > 0) {
		if (emeraldDuration > 0) {
			if (emeraldDuration == EMERALD_DING_GAP_DURATION) {
				ExecuteSFX(CURRENT_BANK, fx_00, SFX_MUTE_MASK(fx_00), SFX_PRIORITY_NORMAL);
			}
			emeraldDuration--;
		} else {
			emeraldDuration = EMERALD_DING_GAP_DURATION;
			emeraldLoop--;
		}
	}
}

UBYTE getMapMetaTileArrayPosition(uint16_t x, uint16_t y) NONBANKED {
	const UBYTE column = LARGE_TILE_FROM_PIXEL(x - mapBoundLeft);
	const UBYTE row = LARGE_TILE_FROM_PIXEL(y - mapBoundUp);
	return row * mapMetaWidth + column;
}

void addOnMap(uint16_t x, uint16_t y, uint8_t metaTile) NONBANKED {
	const UBYTE currentCell = getMapMetaTileArrayPosition(x, y);
	levelMap[currentCell] += metaTile;
}

void runMapSideEffects(void) BANKED {
	const UBYTE currentCell = getMapMetaTileArrayPosition(scroll_target->x, scroll_target->y);
	const UBYTE currentMapValue = levelMap[currentCell];
	if (currentCell == lastVisitedMetaCell && (currentMapValue & metaTileGold) == 0) {
		return;
	}
	if ((currentMapValue & metaTileGold) != 0) {
		const UBYTE x = TILE_FROM_PIXEL(scroll_target->x);
		const UBYTE y = TILE_FROM_PIXEL(scroll_target->y);
		updateScore(scoreGold);
		levelMap[currentCell] &= tunnelMask;
		levelMap[currentCell] |= direction;
		updateVideoMemAndMap(x, y, tileBlack);
		updateVideoMemAndMap(x + 1, y, tileBlack);
		updateVideoMemAndMap(x, y + 1, tileBlack);
		updateVideoMemAndMap(x + 1, y + 1, tileBlack);
	}
	// we eat a gem
	if (currentMapValue == metaTileEmerald) {
		// set current FX to correct note
		fx_00[fxNotePos] = emeraldFreq[emeraldFreqIndex];
		// if we are at the highest frequency, score extra points
		if (emeraldFreqIndex == 7) {
			// reset scale to start
			updateScore(scoreEmerald * 10);
			emeraldFreqIndex = 0;
		} else {	
			// advance one note in the scale for the next emerald
			emeraldFreqIndex++;
		}
		// reset all the emerald timers
		emeraldDuration = EMERALD_DING_GAP_DURATION;
		emeraldScaleTimer = EMERALD_SCALE_TIMER;
		emeraldLoop = EMERALD_DING_QTY;

		updateScore(scoreEmerald);
		diamonds--;
		levelMap[currentCell] = direction;
	} else if ((currentMapValue & tunnelMask) <= 15) {
		// we modify a tunnel flagging the bit of the walkable direction
		// the direction bits have been chosen to match the gameboy const
		// - 8 -   
		// |   |
		// 1   2
		// |   |
		// - 4 -
		levelMap[currentCell] |= direction;
		// we update the previous cell for the opposite direciton, so is a full tunnel
		
		levelMap[lastVisitedMetaCell] |= oppositeDirection;
	}

	// we are under a bag, we need to activate it
	if (currentCell > 14 && (levelMap[currentCell - mapMetaWidth] & metaTileBag)) {
		activateBag(currentCell - mapMetaWidth);
	}
	lastVisitedMetaCell = currentCell;
}

static void resetLevelState(void) {
	lastVisitedMetaCell = 0;
	isDying = 0;
	deathRespawnQueued = FALSE;
	deathRespawnTimer = 0;
	enemyCountOnScreen = 0;
	enemySpawned = 0;
	spawnTimer = 0;
	SpriteManagerReset();
	scroll_target = SpriteManagerAdd(SpritePlayer, 136, 160);
	paintScore();
}

void beginDeathFreeze(void) BANKED {
	isDying = TRUE;
	deathRespawnQueued = FALSE;
	deathRespawnTimer = 0;
	StopMusic;
}

void playDeathMusic(void) BANKED {
	PlayMusic(dirge, 0);
}

void queueDeathRespawn(uint16_t frames) BANKED {
	deathRespawnTimer = frames;
	deathRespawnQueued = TRUE;
}

static void loadLevel(UBYTE level) {
	if (level > 8) {
		currentLevel = 1;
		level = 1;
	}
	resetLevelState();
	if (difficultyLevel < maxDifficultyLevel) {
		difficultyLevel++;
	}
	if (difficultyLevel == 1) {
		enemyMaxOnScreen = maxEnemiesOnScreenLevel1;
	} else if (difficultyLevel < 8) {
		enemyMaxOnScreen = maxEnemiesOnScreenLevel2To7;
	} else {
		enemyMaxOnScreen = maxEnemiesOnScreenLevel8To10;
	}
	enemyMaxTotal = totalEnemiesBaseCount + difficultyLevel;
	// add first the spriteManager only then load the level
	switch (level) {
		case 0:
		    IMPORT_MAP(levelDebug);
			copyLevelMapToRam(&levelDebugMap, BANK(Levels), &levelDebug);
			diamonds = 99;
		break;
		case 1:
			IMPORT_MAP(level1);
			copyLevelMapToRam(&level1Map, BANK(Levels), &level1);
			diamonds = 30;
		break;
		case 2: 
			IMPORT_MAP(level2);
			copyLevelMapToRam(&level2Map, BANK(Levels), &level2);
			diamonds = 41;
		break;
		case 3: 
			IMPORT_MAP(level3);
			copyLevelMapToRam(&level3Map, BANK(Levels), &level3);
			diamonds = 51;
		break;
		case 4: 
			IMPORT_MAP(level4);
			copyLevelMapToRam(&level4Map, BANK(Levels), &level4);
			diamonds = 65;
		break;
		case 5: {
			IMPORT_MAP(level5);
			copyLevelMapToRam(&level5Map, BANK(Levels), &level5);
			diamonds = 77;
		} break;
		case 6: {
			// Level tilemaps are synthesized from levelMap, so later levels can
			// reuse the same MapInfo metadata as long as the dimensions match.
			IMPORT_MAP(level6);
			copyLevelMapToRam(&level6Map, BANK(Levels), &level6);
			diamonds = 52;
		} break;
		case 7: {
			IMPORT_MAP(level7);
			copyLevelMapToRam(&level7Map, BANK(Levels), &level7);
			diamonds = 92;
		} break;
		case 8: {
			IMPORT_MAP(level8);
			copyLevelMapToRam(&level8Map, BANK(Levels), &level8);
			diamonds = 63;
		} break;
		default:
		break;
	}
	spawnTimer = enemyFirstSpawnTimer;
}

void START(void) {
	NR52_REG = 0x80; //Enables sound, you should always setup this first
	NR51_REG = 0xFF; //Enables all channels (left and right)
	NR50_REG = 0x77; //Max volume
	lives = 3;
	currentLevel = debugMode ? 0 : 1;
	loadLevel(currentLevel);
	PlayMusic(popcorn, 1);
	IMPORT_MAP(hud);
	INIT_HUD_EX(hud, 0, 8);
	updateScore(0);
}

void UPDATE(void) {
	if (isDying) {
		if (deathRespawnQueued) {
			if (deathRespawnTimer > 0) {
				deathRespawnTimer--;
			}
			if (deathRespawnTimer == 0) {
				if (lives == 0) {
					SetState(StateGame);
				} else {
					resetLevelState();
					PlayMusic(popcorn, 1);
				}
			}
		}
		return;
	}
	if (spawnTimer > 0) {
		spawnTimer--;
	}
	if (diamonds == 0 || (enemySpawned == enemyMaxTotal && enemyCountOnScreen == 0)) {
		currentLevel++;
		loadLevel(currentLevel);
	}
	if (spawnTimer == 0 && enemyCountOnScreen < enemyMaxOnScreen && enemySpawned < enemyMaxTotal) {
		spawnTimer = getEnemySpawnGapTimer();
		enemyCountOnScreen++;
		enemySpawned++;
		SpriteManagerAdd(SpriteEnemy, 232, 16);
		paintScore();
	}
	updateEmeraldSound();
}
