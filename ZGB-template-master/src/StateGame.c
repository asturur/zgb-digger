#include "Banks/SetAutoBank.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"
#include "MapInfo.h"
#include <gb/gb.h> 
#include <string.h>
#include <Sound.h>
#include "Music.h"

extern const UBYTE direction;

extern const unsigned char level1Map[150];
extern const unsigned char level2Map[150];

DECLARE_MUSIC(music);

UBYTE currentLevel = 0;
UDWORD score = 0;
UBYTE diamonds = 0;

// currently loaded map
unsigned char levelMap[150];

struct MapInfo currentInMemoryLevel;
unsigned char tileMap[768];

void copyTileMapToRam(uint8_t levelToLoadBank, struct MapInfo *levelToLoad) NONBANKED {
	uint8_t __save = CURRENT_BANK;
	SWITCH_ROM(levelToLoadBank);
	// copy everything
	currentInMemoryLevel = *levelToLoad;
	// now copy the actual map data inside the array
	memcpy(tileMap, levelToLoad->data, currentInMemoryLevel.width * currentInMemoryLevel.height);
	// redefine the pointer to my in memory array
	currentInMemoryLevel.data = tileMap;
	// if (levelToLoad->attributes) memcpy(map_attr, levelToLoad->attributes, current_level.width * current_level.height); else memset(map_attr, 0, sizeof(map_attr));
	SWITCH_ROM(__save);
}

void copyLevelMapToRam(unsigned char *mapToLoad[]) NONBANKED {
	// uint8_t __save = CURRENT_BANK;
	// SWITCH_ROM(mapToLoadBank);
	memcpy(levelMap, mapToLoad, 150);
	// SWITCH_ROM(__save);
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
		// to be done
	}
}



void loadLevel(UBYTE level) {
	switch (level) {
		case 1:
			IMPORT_MAP(level1);
			copyTileMapToRam(BANK(level1), &level1);
			copyLevelMapToRam(&level1Map);
			InitScroll(BANK(level1), &currentInMemoryLevel, 0, 0);
			diamonds = 30;
		break;
		case 2: 
			IMPORT_MAP(level2);
			copyTileMapToRam(BANK(level2), &level2);
			copyLevelMapToRam(&level2Map);
			InitScroll(BANK(level2), &currentInMemoryLevel, 0, 0);
			diamonds = 41;
		break;
		default:
		  currentLevel = 1;
		  loadLevel(currentLevel)
	}
}

void START() {
	NR52_REG = 0x80; //Enables sound, you should always setup this first
	NR51_REG = 0xFF; //Enables all channels (left and right)
	NR50_REG = 0x77; //Max volume
	// add first the spriteManager only then load the level
	scroll_target = SpriteManagerAdd(SpritePlayer, 136, 168);
	loadLevel(1);
	PlayMusic(music, 1);
}

void UPDATE() {
	if (diamonds == 0) {
		currentLevel++;
		loadLevel(currentLevel);
	}
}
