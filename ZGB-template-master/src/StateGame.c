#include "Banks/SetAutoBank.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"
#include "MapInfo.h"
#include <gb/gb.h> 
#include <string.h>
#include <Sound.h>
#include "Music.h"

#define tilesPerRow 32
#define maxEnimesCount 3
#define enemySpawnTimer 300

extern const UBYTE direction;

extern const unsigned char level1Map[150];
extern const unsigned char level2Map[150];
extern const unsigned char level3Map[150];

DECLARE_MUSIC(music);

UBYTE currentLevel = 0;
UDWORD score = 0;
UBYTE diamonds = 0;
uint16_t spawnTimer = 0;
uint8_t enemyCount = 0;

// currently loaded map
unsigned char levelMap[150];

struct MapInfo currentInMemoryLevel;
unsigned char tileMap[768];

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
	// if (levelToLoad->attributes) memcpy(map_attr, levelToLoad->attributes, current_level.width * current_level.height); else memset(map_attr, 0, sizeof(map_attr));
	SWITCH_ROM(__save);
}

void copyLevelMapToRam(unsigned char *mapToLoad[]) NONBANKED {
	// uint8_t __save = CURRENT_BANK;
	// SWITCH_ROM(mapToLoadBank);
	memcpy(levelMap, mapToLoad, 150);
	int8_t i, j;
	// fill up the first 3 lines of the map with 2 lines 0s and 1 of 1s
	memset(tileMap, 0, tilesPerRow * 2);
	memset(tileMap + tilesPerRow * 2, 1, tilesPerRow * 22);
	uint8_t metaTile;
	// third row start at 96, we skip the first tile
	uint16_t offset = tilesPerRow * 3 + 1;

	// loop the map
	for (i = 0; i < 10; i++) {
		for (j = 0; j < 15; j++) {
			metaTile = levelMap[i * 15 + j];
			if (metaTile > 0 && metaTile < 16) {
				// fill in four 0 tiles, the black gallery
				// at current row
				tileMap[offset] = 0;
				tileMap[offset + 1] = 0;
				// at next row
				tileMap[offset + tilesPerRow] = 0;
				tileMap[offset + tilesPerRow + 1] = 0;
			} else if (metaTile == 16) {
				tileMap[offset] = 2;
				tileMap[offset + 1] = 3;
				// at next row
				tileMap[offset + tilesPerRow] = 4;
				tileMap[offset + tilesPerRow + 1] = 5;
			} else if (metaTile == 17) {
				tileMap[offset] = 6;
				tileMap[offset + 1] = 7;
				// at next row
				tileMap[offset + tilesPerRow] = 8;
				tileMap[offset + tilesPerRow + 1] = 9;
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
	uint8_t positionX = 8 + column * 16;
	uint8_t positionY = 24 + row * 16;
	SpriteManagerAdd(SpriteBag, positionX, positionY);

}

void runMapSideEffects() {
	const UBYTE column = (scroll_target->x - ((scroll_target->x - 8) % 16) - 8) / 16;
	const UBYTE row = (scroll_target->y - ((scroll_target->y - 24) % 16) - 24) / 16;
	const UBYTE currentCell = row * 15 + column;
	const UBYTE currentMapValue = levelMap[currentCell];

	// we eat a gem
	if (currentMapValue == 16) {
		// took from a tutorial
		PlayFx(CHANNEL_1, 10, 0x4f, 0xc7, 0xf3, 0x73, 0x86);
		score += 50;
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

void resetLevelState() {
	enemyCount = 0;
	spawnTimer = enemySpawnTimer;
	SpriteManagerReset();
}

void loadLevel(UBYTE level) {
	resetLevelState();
	// add first the spriteManager only then load the level
	scroll_target = SpriteManagerAdd(SpritePlayer, 136, 168);
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

void START() {
	NR52_REG = 0x80; //Enables sound, you should always setup this first
	NR51_REG = 0xFF; //Enables all channels (left and right)
	NR50_REG = 0x77; //Max volume
	currentLevel = 1;
	loadLevel(currentLevel);
	PlayMusic(music, 1);
}

void UPDATE() {
	spawnTimer--;
	if (diamonds == 0) {
		currentLevel++;
		loadLevel(currentLevel);
	}
	if (spawnTimer == 0 && enemyCount < maxEnimesCount) {
		enemyCount++;
		SpriteManagerAdd(SpriteEnemy, 232, 24);
		spawnTimer = 500;
	}
}
