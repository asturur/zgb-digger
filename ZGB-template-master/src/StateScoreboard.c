#include "Banks/SetAutoBank.h"

#include <gb/gb.h>
#include "Keys.h"
#include "Print.h"
#include "Scroll.h"
#include "StateGame.h"
#include "ZGBMain.h"
#include "SpriteEnemy.h"
#include "SpritePlayer.h"
#include "savegame.h"
#include "StateNameEntry.h"

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
#define spawnBonus (spawnEmeraldText + 30)
#define spawnBonusText (spawnBonus + 60)

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
	OBP0_REG = DMG_PALETTE(DMG_WHITE, DMG_LITE_GRAY, DMG_DARK_GRAY, DMG_BLACK); // normal palette
	OBP1_REG = DMG_PALETTE(DMG_BLACK, DMG_WHITE, DMG_LITE_GRAY, DMG_DARK_GRAY); // bright palette
	enemyNob = 0;
	enemyHob = 0;
	digger = 0;
	splash_timer = 0;
	move_bkg(0, 0);

	InitScroll(BANK(scoreBoard), &scoreBoard, 0, 0);
	MoveScroll(0, 0);
	INIT_FONT(font, PRINT_BKG);

	PRINT(startLeft, 3, " HI SCORES");

	ENABLE_RAM;

	for (uint8_t i = 0; i < 10; i++) {
		if (savegame.hiscores[i].initials[0] != 0xFF && savegame.hiscores[i].initials[3] != 0xFF) {
			PRINT(startLeft, i + 5, "%s", savegame.hiscores[i].initials);
			printScoreOnScreen(savegame.hiscores[i].score, startLeft + 4, i + 5);
		} else {
			PRINT(startLeft, i + 5, "...      0");
		}
	}


	DISABLE_RAM;

	// PRINT(11, 10, "  GOLD");
	// PRINT(11, 12, "  EMERALD");
	// PRINT(11, 14, "  BONUS");
}
UBYTE spriteStaticMode = 1; 

void UPDATE(void) {
	if (splash_timer < 60000) {
	  splash_timer++;
	} else {
	  return;
	}

	switch (splash_timer) {
		case spawnNob: 
			enemyNob = SpriteManagerAdd(SpriteEnemy, leftSpawnPoint, 28);
			if (enemyNob != 0) {
				setEnemyModeFor(enemyNob, scoreboardNobMode);
			}
		break;
		case spawnNobText:
			PRINT(startRight, 4, "  NOBBIN");
		break;
		case spawnHob:
			enemyHob = SpriteManagerAdd(SpriteEnemy, leftSpawnPoint, 28 + 16);
			if (enemyHob != 0) {
				setEnemyModeFor(enemyHob, scoreboardHobMode);
			}
		break;
		case spawnHobText:
			PRINT(startRight, 6, "  HOBBIN");
		break;
		case spawnDigger:
			digger = SpriteManagerAdd(SpritePlayer, leftSpawnPoint, 28 + 16 * 2);
			if (digger != 0) {
				digger->custom_data[death_state] = playerDeathScoreboardMode;
				SetSpriteAnim(digger, anim_walk_left, 15);
			}
		break;
		case spawnDiggerText:
			if (digger != 0) {
				SetSpriteAnim(digger, anim_walk_right, 15);
			}
			PRINT(startRight, 8, "  DIGGER");
		break;
		case spawnGold:
		    spriteStaticMode = 2;
			SpriteManagerAddEx(SpriteStaticobj, leftDestination - 1, 28 + 16 * 3, &spriteStaticMode);
		break;
		case spawnGoldText:
			PRINT(startRight, 10, "  GOLD");
		break;
		case spawnEmerald:
			spriteStaticMode = 1;
			SpriteManagerAddEx(SpriteStaticobj, leftDestination - 1, 28 + 16 * 4, &spriteStaticMode);
		break;
		case spawnEmeraldText:
			PRINT(startRight, 12, "  EMERALD");
		break;
		case spawnBonus:
			spriteStaticMode = 3;
			SpriteManagerAddEx(SpriteStaticobj, leftDestination - 1, 28 + 16 * 5, &spriteStaticMode);
		break;
		case spawnBonusText:
			PRINT(startRight, 14, "  BONUS");
		break;
	}

	if (enemyNob != 0 && enemyNob->x >= leftDestination) {
		enemyNob->custom_data[enemy_movement_accumulator] += 4;
    	if (enemyNob->custom_data[enemy_movement_accumulator] >= 25) {
        	enemyNob->custom_data[enemy_movement_accumulator] -= 5;
			enemyNob->x--;
    	}
	}
	if (enemyHob != 0 && enemyHob->x >= leftDestination) {
		enemyHob->custom_data[enemy_movement_accumulator] += 4;
    	if (enemyHob->custom_data[enemy_movement_accumulator] >= 25) {
        	enemyHob->custom_data[enemy_movement_accumulator] -= 5;
			enemyHob->x--;
    	}
	}	
	if (digger != 0 && digger->x >= leftDestination) {
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
