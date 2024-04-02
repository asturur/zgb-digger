#include "Banks/SetAutoBank.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"
#include "MapInfo.h"
#include <gb/gb.h> 
#include <string.h>
#include <Sound.h>
#include "Music.h"
#include "StateGame.h"
#include "SpritePlayer.h"

extern const UBYTE direction;

extern const unsigned char level1Map[150];
extern const unsigned char level2Map[150];
extern const unsigned char level3Map[150];

UBYTE currentLevel = 0;
uint16_t score = 0;
UBYTE diamonds = 0;
uint16_t spawnTimer = 0;
uint8_t enemyCount = 0;
uint8_t lives = 3;

uint8_t emeraldLoop = 0;
uint8_t emeraldDuration = 0;
uint8_t emeraldFreq[] = { 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95 };

// handles the death state.
// sprites do not move, a music plays
uint8_t isDying = 0;

DECLARE_MUSIC(music);

// currently loaded map
unsigned char levelMap[150];

struct MapInfo currentInMemoryLevel;
unsigned char tileMap[736];

uint8_t getTilefromPosition(uint8_t posX) {
    return (posX - (posX % tileSize)) >> tileSizeBitShift;
}

void killPlayer(void) {
	lives--;
	isDying = 128;
	scroll_target->custom_data[death_animation] = death_sequence_length;
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
	for (uint8_t i = 1; i <= lives; i++) {
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

void copyLevelMapToRam(unsigned char *mapToLoad[]) NONBANKED {
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
	for (i = 0; i < 10; i++) {
		for (j = 0; j < 15; j++) {
			metaTile = levelMap[i * 15 + j];
			if (metaTile > 0 && metaTile < 16) {
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
}

void activateBag(uint8_t bagcell) {
	// remove the bag tiles and replace with grass.
	// remove the bag from the map replace with grass
	// activate bag sprite
	levelMap[bagcell] = 0;
	uint8_t column = bagcell % 15;
	uint8_t row = (bagcell - column) / 15;
	uint8_t positionX = mapBoundLeft + column * 16;
	uint8_t positionY = mapBoundUp + row * 16;
	SpriteManagerAdd(SpriteBag, positionX, positionY);
}

void updateScore(uint16_t addScore) {
	score += addScore;
	paintScore();
}

void playEmeraldSound(void) {
	if (emeraldDuration > 0) {
		if (emeraldDuration == 7 || emeraldDuration == 6) {
			PlayFx(CHANNEL_1, 10, 0x00, 0x81, 0x84, 0x95, 0xc6);
		}
		emeraldDuration--;
	} else {
		if (emeraldLoop > 0) {
			emeraldDuration = 7;
			emeraldLoop--;
		}
	}
}

void runMapSideEffects(void) {
	const UBYTE column = (scroll_target->x - ((scroll_target->x - mapBoundLeft) % 16) - 8) / 16;
	const UBYTE row = (scroll_target->y - ((scroll_target->y - mapBoundUp) % 16) - 16) / 16;
	const UBYTE currentCell = row * 15 + column;
	const UBYTE currentMapValue = levelMap[currentCell];

	// we eat a gem
	if (currentMapValue == 16) {
		// took from a tutorial
		PlayFx(CHANNEL_1, 10, 0x4f, 0xc7, 0xf3, 0x73, 0x86);
		emeraldDuration = 7;
		emeraldLoop = 7;
		updateScore(scoreEmerald);
		diamonds--;
		if (direction == J_RIGHT) {
			levelMap[currentCell] = 8;
		}
		if (direction == J_LEFT) {
			levelMap[currentCell] = 2;
		}
		if (direction == J_UP) {
			levelMap[currentCell] = 4;
		}
		if (direction == J_DOWN) {
			levelMap[currentCell] = 1;
		}
	} else if (currentMapValue <= 15) {
	// we modify a tunnel flagging the bit of the walkable direction
		if (direction == J_RIGHT) {
			levelMap[currentCell] &= 8;
		}
		if (direction == J_LEFT) {
			levelMap[currentCell] &= 2;
		}
		if (direction == J_UP) {
			levelMap[currentCell] &= 4;
		}
		if (direction == J_DOWN) {
			levelMap[currentCell] &= 1;
		}
	}

	// we are under a bag, we need to activate it
	if (currentCell > 14 && levelMap[currentCell - 15] == 17) {
		activateBag(currentCell - 15);
	}
}

void resetLevelState(void) {
	isDying = 0;
	enemyCount = 0;
	spawnTimer = enemySpawnTimer;
	SpriteManagerReset();
	scroll_target = SpriteManagerAdd(SpritePlayer, 136, 160);
}

void loadLevel(UBYTE level) {
	resetLevelState();
	// add first the spriteManager only then load the level
	switch (level) {
		case 1:
			IMPORT_MAP(level1);
			copyLevelMapToRam(&level1Map);
			copyTileMapToRam(BANK(level1), &level1);
			InitScroll(BANK(level1), &currentInMemoryLevel, 0, 0);
			diamonds = 30;
		break;
		case 2: 
			IMPORT_MAP(level2);
			copyLevelMapToRam(&level2Map);
			copyTileMapToRam(BANK(level2), &level2);
			InitScroll(BANK(level2), &currentInMemoryLevel, 0, 0);
			diamonds = 41;
		break;
		case 3: 
			IMPORT_MAP(level3);
			copyLevelMapToRam(&level3Map);
			copyTileMapToRam(BANK(level3), &level3);
			InitScroll(BANK(level3), &currentInMemoryLevel, 0, 0);
			diamonds = 55;
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
	
	currentLevel = 1;
	loadLevel(currentLevel);
	PlayMusic(music, 1);
	IMPORT_MAP(hud);
	INIT_HUD_EX(hud, 0, 8);
	paintScore();
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
	if (spawnTimer == 0 && enemyCount < maxEnimesCount) {
		spawnTimer = enemySpawnTimer;
		enemyCount++;
		SpriteManagerAdd(SpriteEnemy, 232, 16);
		paintScore();
	}
	// playEmeraldSound();
}

