#include "Banks/SetAutoBank.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"



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
	4,  0, 0,  0, 17, 0,  0,  0,  0,  0, 14, 10, 10, 10,  8, // "S   B     HHHHS", 
    5,  0, 0, 16, 16, 0,  0, 16,  0,  0,  5,  0, 17,  0,  0, // "V  CC  C  V B  "
	5, 17, 0, 16, 16, 0,  0, 16,  0,  0,  5,  0,  0,  0,  0, // "VB CC  C  V    ",
    5,  0, 0, 16, 16, 17, 0, 16, 17,  0,  5,  0, 16, 16, 16, // "V  CCB CB V CCC",
    5,  0, 0, 16, 16,  0, 0, 16,  0,  0,  5,  0, 16, 16, 16, // "V  CC  C  V CCC",
	3, 10, 0, 16, 16,  0, 0, 16,  0,  0,  5,  0, 16, 16, 16, // "HH CC  C  V CCC",

//   " V    B B V    ",
//   " HHHH     V    ",
//   "C   V     V   C",
//   "CC  HHHHHHH  CC"
}

void eatDiamond() {

}

void loadLevel(UBYTE level) {
	switch (level) {
		case 1: 
			IMPORT_MAP(level1);
			InitScroll(BANK(level1), &level1, 0, 0);
			diamonds = 30;
		break;
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
}
