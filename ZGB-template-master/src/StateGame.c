#include "Banks/SetAutoBank.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"
#include "MapInfo.h"
#include <gb/gb.h> 
#include <string.h>
extern const UBYTE direction;

UBYTE currentLevel = 0;
UDWORD score = 0;
UBYTE diamonds = 0;
BOOLEAN changeLevel = FALSE;
// map descriptors
// 0 grass
// 1-15 walkable tunnel 5 is a V, 10 is H, 
// 16 diamond
// 17 bag
UBYTE map[] = {
	4,  0,  0,  0, 17,  0,  0,  0,  0,  0, 14, 10, 10, 10,  8, // "S   B     HHHHS", 
    5,  0,  0, 16, 16,  0,  0, 16,  0,  0,  5,  0, 17,  0,  0, // "V  CC  C  V B  "
	5, 17,  0, 16, 16,  0,  0, 16,  0,  0,  5,  0,  0,  0,  0, // "VB CC  C  V    ",
    5,  0,  0, 16, 16, 17,  0, 16, 17,  0,  5,  0, 16, 16, 16, // "V  CCB CB V CCC",
    5,  0,  0, 16, 16,  0,  0, 16,  0,  0,  5,  0, 16, 16, 16, // "V  CC  C  V CCC",
	3, 10,  0, 16, 16,  0,  0, 16,  0,  0,  5,  0, 16, 16, 16, // "HH CC  C  V CCC",
	0,  5,  0,  0,  0,  0, 17,  0, 17,  0,  5,  0,  0,  0,  0, // " V    B B V    ",
    0,  3, 10, 10, 12,  0,  0,  0,  0,  0,  5,  0,  0,  0,  0, // " HHHH     V    ",
   16,  0,  0,  0,  5,  0,  0,  0,  0,  0,  5,  0,  0,  0, 16, // "C   V     V   C",
   16, 16,  0,  0,  3, 10, 10, 10, 10, 10,  9,  0,  0, 16, 16, // "CC  HHHHHHH  CC",
};


/*
uint8_t map_buffer[MAX_WIDTH * MAX_HEIGHT]; // tile map in RAM 2KB
uint8_t map_attr[MAX_WIDTH * MAX_HEIGHT];   // tile attributes in RAM 2KB

void copyMapToRam(uint8_t levelToLoadBank, struct MapInfo *levelToLoad) NONBANKED {
    uint8_t __save = CURRENT_BANK;
    SWITCH_ROM(levelToLoadBank);
    // copy everything
    current_level = *levelToLoad;
    current_level.data = map_buffer;
    current_level.attributes = map_attr;    
    // now copy the actual map data inside the array
    memcpy(map_buffer, levelToLoad->data, current_level.width * current_level.height);
    if (levelToLoad->attributes) memcpy(map_attr, levelToLoad->attributes, current_level.width * current_level.height); else memset(map_attr, 0, sizeof(map_attr));
    // restore bank
    SWITCH_ROM(__save);
}

*/

struct MapInfo currentInMemoryLevel;
unsigned char tileMap[768];

void copyMapToRam(uint8_t levelToLoadBank, struct MapInfo *levelToLoad) NONBANKED {
	uint8_t __save = CURRENT_BANK;
	SWITCH_ROM(levelToLoadBank);
	// copy everything
	currentInMemoryLevel = *levelToLoad;
	// redefine the pointer to my in memory array
	currentInMemoryLevel.data = tileMap;
	// now copy the actual map data inside the array
	memcpy(&tileMap, levelToLoad->data, currentInMemoryLevel.width * currentInMemoryLevel.height);
	// if (levelToLoad->attributes) memcpy(map_attr, levelToLoad->attributes, current_level.width * current_level.height); else memset(map_attr, 0, sizeof(map_attr));
	SWITCH_ROM(__save);
}


void runMapSideEffects() {
	const UBYTE column = (scroll_target->x - (scroll_target->x % 16) - 8) / 16;
	const UBYTE row = (scroll_target->y - (scroll_target->y % 16) - 24) / 16;
	const UBYTE currentCell = row * 15 + column;
	const UBYTE currentMapValue = map[currentCell];

	// we eat a gem
	if (currentMapValue == 16) {
		score += 50;
		diamonds--;
		if (direction == J_RIGHT) {
			map[currentCell] = 8;
		}
		if (direction == J_LEFT) {
			map[currentCell] = 2;
		}
		if (direction == J_UP) {
			map[currentCell] = 4;
		}
		if (direction == J_DOWN) {
			map[currentCell] = 1;
		}
	} else if (currentMapValue <= 15) {
	// we modify a tunnel flagging the bit of the walkable direction
		if (direction == J_RIGHT) {
			map[currentCell] &= 8;
		}
		if (direction == J_LEFT) {
			map[currentCell] &= 2;
		}
		if (direction == J_UP) {
			map[currentCell] &= 4;
		}
		if (direction == J_DOWN) {
			map[currentCell] &= 1;
		}
	}

	// we are under a bag, we need to activate it
	if (currentCell > 14 && map[currentCell - 15] == 17) {
		// to be done
	}
}



void loadLevel(UBYTE level) {
	switch (level) {
		case 1:
			IMPORT_MAP(level1);
			copyMapToRam(BANK(level1), &level1);
			InitScroll(BANK(level1), &currentInMemoryLevel, 0, 0);
			diamonds = 30;
		break;
		// case 2: 
		// 	IMPORT_MAP(level2);
		// 	InitScroll(BANK(level2), &level2, 0, 0);
		// 	diamonds = 30;
		// break;
	}
}

void START() {
	scroll_target = SpriteManagerAdd(SpritePlayer, 136, 168);
}

void UPDATE() {
	if (diamonds == 0) {
		currentLevel++;
		loadLevel(currentLevel);
	}
	runMapSideEffects();
}
