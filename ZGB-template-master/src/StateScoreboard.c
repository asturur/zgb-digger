#include "Banks/SetAutoBank.h"

#include <gb/gb.h>
#include "Keys.h"
#include "Print.h"
#include "Scroll.h"
#include "StateGame.h"
#include "ZGBMain.h"
#include "SpriteEnemy.h"

#define CENTER(len) ((SCREEN_TILES_W - (len)) >> 1)
#define startRight 11
#define startLeft 0

#define spawnNob 30
#define spwanNobText 180

IMPORT_TILES(font);
IMPORT_MAP(scoreBoard);

static uint16_t scoreTimer = 0;
static Sprite* enemyNob;
static Sprite* enemyHob;

void START(void) {
	scoreTimer = 0;
	move_bkg(0, 0);

	InitScroll(BANK(scoreBoard), &scoreBoard, 0, 0);
	MoveScroll(0, 0);
	INIT_FONT(font, PRINT_BKG);

	PRINT(startLeft, 4, "HI SCORES");
	PRINT(startLeft, 4, "... 000000");
	PRINT(startLeft, 5, "... 000000");
	PRINT(startLeft, 6, "... 000000");
	PRINT(startLeft, 7, "... 000000");
	PRINT(startLeft, 8, "... 000000");
	PRINT(startLeft, 9, "... 000000");
	PRINT(startLeft, 10, "... 000000");
	PRINT(startLeft, 11, "... 000000");
	PRINT(startLeft, 12, "... 000000");
	PRINT(startLeft, 13, "... 000000");
    PRINT(startLeft, 14, "... 000000");

	// PRINT(11, 4, "  NOBBIN");
	// PRINT(11, 6, "  HOBBIN");
	// PRINT(11, 8, "  DIGGER");
	// PRINT(11, 10, "  GOLD");
	// PRINT(11, 12, "  EMERALD");
	// PRINT(11, 14, "  BONUS");
}

void UPDATE(void) {
	if (scoreTimer < 60000) {
	  scoreTimer++;
	} else {
	  return;
	}

	if (scoreTimer == spawnNob) {
		enemyNob = SpriteManagerAdd(SpriteEnemy, 144, 32);
		setEnemyModeFor(enemyNob, scoreboardNobMode);
	}

	if(KEY_TICKED(J_START)) {
		SetState(StateGame);
		return;
	}
	if(KEY_TICKED(J_SELECT)) {
		SetState(StateOptions);
		return;
	}
	if(KEY_TICKED(J_A | J_B)) {
		SetState(StateMenu);
	}
}

void DESTROY(void) {
}
