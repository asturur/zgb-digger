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

extern const unsigned char level1Map[150];
extern const unsigned char level2Map[150];
extern const unsigned char level3Map[150];
extern const unsigned char level4Map[150];
extern const unsigned char level5Map[150];

extern uint8_t fx_00[];
extern void __mute_mask_fx_00;

UBYTE currentLevel = 0;
uint16_t score = 0;
UBYTE diamonds = 0;
uint16_t spawnTimer = 0;
uint8_t enemyCount = 0;
uint8_t lives = 3;

uint8_t emeraldLoop = EMERALD_DING_QTY;
uint8_t emeraldDuration = EMERALD_DING_GAP_DURATION;
uint8_t emeraldScaleTimer = 0;
uint8_t emeraldFreq[] = { fxC_4, fxD_4, fxE_4, fxF_4, fxG_4, fxA_4, fxB_4, fxC_5 };
uint8_t emeraldFreqIndex = 0;

uint8_t lastVisitedMetaCell = 0;

// handles the death state.
// sprites do not move, a music plays
uint8_t isDying = 0;

DECLARE_MUSIC(music);

// currently loaded map
unsigned char levelMap[150];

struct MapInfo currentInMemoryLevel;
unsigned char tileMap[736];

void killPlayer(void) {
	lives--;
	isDying = 128;
	scroll_target->custom_data[death_animation] = death_sequence_length;
	if (lives == 0) {
		SetState(StateGame);
	}
}

void updateVideoMemAndMap(UBYTE column, UBYTE row, UBYTE type) {
	set_bkg_tile_xy(column, row, type);
    tileMap[row * tilesPerRow + column] = type;
}

BOOLEAN checkTilesFor(UBYTE column, UBYTE row, UBYTE type) {
    return get_bkg_tile_xy(column, row) == type ||
        get_bkg_tile_xy(column + 1, row) == type || 
        get_bkg_tile_xy(column, row + 1) == type || 
        get_bkg_tile_xy(column + 1, row + 1) == type;
}

void paintScore(void) {
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
	UPDATE_HUD_TILE(14, 0, scoreFontOffset + enemyCount);
}

void copyTileMapToRam(uint8_t levelToLoadBank, struct MapInfo *levelToLoad) NONBANKED {
	uint8_t __save = CURRENT_BANK;
	SWITCH_ROM(levelToLoadBank);
	// copy everything
	currentInMemoryLevel = *levelToLoad;
	// the data in the array is filled in by copyLevelMapToRam
	// now copy the actual map data inside the array
	// memcpy(tileMap, levelToLoad->data, currentInMemoryLevel.width * currentInMemoryLevel.height);
	// redefine the pointer to my in memory array
	currentInMemoryLevel.data = tileMap;
	currentInMemoryLevel.width = 32;
	currentInMemoryLevel.height = 23;
	// if (levelToLoad->attributes) memcpy(map_attr, levelToLoad->attributes, current_level.width * current_level.height); else memset(map_attr, 0, sizeof(map_attr));
	SWITCH_ROM(__save);
}

void copyLevelMapToRam(unsigned char *mapToLoad[], uint8_t levelToLoadBank, struct MapInfo *levelToLoad) NONBANKED {
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

void activateBag(uint8_t bagcell) {
	// remove the bag tiles and replace with grass.
	// remove the bag from the map replace with grass
	// activate bag sprite
	levelMap[bagcell] = 0;
	uint8_t column = bagcell % mapMetaWidth;
	uint8_t row = (bagcell - column) / mapMetaWidth;
	uint8_t positionX = mapBoundLeft + column * 16;
	uint8_t positionY = mapBoundUp + row * 16;
	SpriteManagerAdd(SpriteBag, positionX, positionY);
}

void updateScore(uint16_t addScore) {
	score += addScore;
	paintScore();
}

void updateEmeraldSound(void) {
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

UBYTE getMapMetaTileArrayPosition(uint16_t x, uint16_t y) {
	const UBYTE column = LARGE_TILE_FROM_PIXEL(x - mapBoundLeft);
	const UBYTE row = LARGE_TILE_FROM_PIXEL(y - mapBoundUp);
	return row * mapMetaWidth + column;
}

void addOnMap(uint16_t x, uint16_t y, uint8_t metaTile) {
	const UBYTE currentCell = getMapMetaTileArrayPosition(x, y);
	levelMap[currentCell] = metaTile;
}

void runMapSideEffects(void) {
	const UBYTE currentCell = getMapMetaTileArrayPosition(scroll_target->x, scroll_target->y);
	const UBYTE currentMapValue = levelMap[currentCell];

	if (currentCell == lastVisitedMetaCell) {
		return;
	}
	if (currentMapValue == metaTileGold) {
		updateScore(scoreGold);
		levelMap[currentCell] |= direction;
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
	} else if (currentMapValue <= 15) {
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
	if (currentCell > 14 && levelMap[currentCell - mapMetaWidth] == metaTileBag) {
		activateBag(currentCell - mapMetaWidth);
	}
	lastVisitedMetaCell = currentCell;
}

void resetLevelState(void) {
	lastVisitedMetaCell = 0;
	isDying = 0;
	enemyCount = 0;
	spawnTimer = enemySpawnTimer;
	SpriteManagerReset();
	scroll_target = SpriteManagerAdd(SpritePlayer, 136, 160);
	paintScore();
}

void loadLevel(UBYTE level) {
	resetLevelState();
	// add first the spriteManager only then load the level
	switch (level) {
		case 1:
			IMPORT_MAP(level1);
			copyLevelMapToRam(&level1Map, BANK(level1), &level1);
			diamonds = 30;
		break;
		case 2: 
			IMPORT_MAP(level2);
			copyLevelMapToRam(&level2Map, BANK(level2), &level2);
			diamonds = 41;
		break;
		case 3: 
			IMPORT_MAP(level3);
			copyLevelMapToRam(&level3Map, BANK(level3), &level3);
			diamonds = 51;
		break;
		case 4: 
			IMPORT_MAP(level4);
			copyLevelMapToRam(&level4Map, BANK(level4), &level4);
			diamonds = 65;
		break;
		case 5: 
			IMPORT_MAP(level5);
			copyLevelMapToRam(&level5Map, BANK(level5), &level5);
			diamonds = 77;
		break;
		default:
		  currentLevel = 1;
		  loadLevel(currentLevel);
	}
}

void START(void) {
	NR52_REG = 0x80; //Enables sound, you should always setup this first
	NR51_REG = 0xFF; //Enables all channels (left and right)
	NR50_REG = 0x77; //Max volume
	lives = 3;
	currentLevel = 1;
	loadLevel(currentLevel);
	PlayMusic(music, 1);
	IMPORT_MAP(hud);
	INIT_HUD_EX(hud, 0, 8);
	updateScore(0);
}

void UPDATE(void) {
	if (isDying > 0) {
		isDying--;
		if (isDying == 0) {
			resetLevelState();
		}
		return;
	}
	if (spawnTimer > 0) {
		spawnTimer--;
	}
	if (diamonds == 0) {
		currentLevel++;
		loadLevel(currentLevel);
	}
	// if (spawnTimer == 0 && enemyCount < maxEnimesCount) {
	// 	spawnTimer = enemySpawnTimer;
	// 	enemyCount++;
	// 	SpriteManagerAdd(SpriteEnemy, 232, 16);
	// 	paintScore();
	// }
	updateEmeraldSound();
}

