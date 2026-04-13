#include "Banks/SetAutoBank.h"

#include <gb/gb.h>
#include "Keys.h"
#include "Print.h"
#include "Scroll.h"
#include "StateGame.h"
#include "ZGBMain.h"
#include "SpriteEnemy.h"
#include "SpritePlayer.h"

#define CENTER(len) ((SCREEN_TILES_W - (len)) >> 1)
#define startRight 11
#define startLeft 0
#define leftSpawnPoint 144
#define leftDestination 88

#define spawnNob 30
#define spawnNobText (spawnNob + 120)
#define spawnHob (spawnNobText + 30)
#define spawnHobText (spawnHob + 120)
#define spawnDigger (spawnHobText + 30)
#define spawnDiggerText (spawnDigger + 120)
#define spawnGold (spawnDiggerText + 30)
#define spawnGoldText (spawnGold + 60)
#define spawnEmerald (spawnGoldText + 30)
#define spawnEmeraldText (spawnEmerald + 60)

IMPORT_TILES(font);
IMPORT_MAP(scoreBoard);

BANKREF_EXTERN(SpritePlayer)
extern const UBYTE anim_walk_left[5];
extern const UBYTE anim_walk_right[5];

extern UINT16 splash_timer;

static Sprite* enemyNob;
static Sprite* enemyHob;
static Sprite* digger;

void START(void) {
	OBP1_REG = DMG_PALETTE(DMG_BLACK, DMG_WHITE, DMG_LITE_GRAY, DMG_DARK_GRAY); // bright palette
	splash_timer = 0;
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

	// PRINT(11, 10, "  GOLD");
	// PRINT(11, 12, "  EMERALD");
	// PRINT(11, 14, "  BONUS");
}

void UPDATE(void) {
	if (splash_timer < 60000) {
	  splash_timer++;
	} else {
	  return;
	}

	switch (splash_timer) {
		case spawnNob: 
			enemyNob = SpriteManagerAdd(SpriteEnemy, leftSpawnPoint, 28);
			setEnemyModeFor(enemyNob, scoreboardNobMode);
		break;
		case spawnNobText:
			PRINT(startRight, 4, "  NOBBIN");
		break;
		case spawnHob:
			enemyHob = SpriteManagerAdd(SpriteEnemy, leftSpawnPoint, 28 + 16);
			setEnemyModeFor(enemyHob, scoreboardHobMode);
		break;
		case spawnHobText:
			PRINT(startRight, 6, "  HOBBIN");
		break;
		case spawnDigger:
			digger = SpriteManagerAdd(SpritePlayer, leftSpawnPoint, 28 + 16 * 2);
			digger->custom_data[death_state] = playerDeathScoreboardMode;
			SetSpriteAnim(digger, anim_walk_left, 15);
		break;
		case spawnDiggerText:
		SetSpriteAnim(digger, anim_walk_right, 15);
			PRINT(startRight, 8, "  DIGGER");
		break;
		case spawnGold:

		break;
		case spawnGoldText:
		SetSpriteAnim(digger, anim_walk_right, 15);
			PRINT(startRight, 10, "  GOLD");
		break;
		case spawnEmerald:

		break;
		case spawnEmeraldText:
			PRINT(startRight, 12, "  EMERALD");
		break;
	}

	if (enemyNob->x >= leftDestination) {
		enemyNob->custom_data[enemy_movement_accumulator] += 4;
    	if (enemyNob->custom_data[enemy_movement_accumulator] >= 25) {
        	enemyNob->custom_data[enemy_movement_accumulator] -= 5;
			enemyNob->x--;
    	}
	}
	if (enemyHob->x >= leftDestination) {
		enemyHob->custom_data[enemy_movement_accumulator] += 4;
    	if (enemyHob->custom_data[enemy_movement_accumulator] >= 25) {
        	enemyHob->custom_data[enemy_movement_accumulator] -= 5;
			enemyHob->x--;
    	}
	}	
	if (digger->x >= leftDestination) {
		digger->custom_data[player_movement_accumulator] += 4;
    	if (digger->custom_data[player_movement_accumulator] >= 25) {
        	digger->custom_data[player_movement_accumulator] -= 5;
			digger->x--;
    	}
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
