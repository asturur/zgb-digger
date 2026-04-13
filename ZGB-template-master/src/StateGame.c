#include "Banks/SetAutoBank.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"
#include "MapInfo.h"
#include <gb/gb.h> 
#include <string.h>
#include <Sound.h>
#include "Sounds.h"
#include "Keys.h"
#include "Music.h"
#include "StateGame.h"
#include "SpriteBag.h"
#include "SpritePlayer.h"

IMPORT_MAP(levelDebug);
IMPORT_MAP(level1);
IMPORT_MAP(level2);
IMPORT_MAP(level3);
IMPORT_MAP(level4);
IMPORT_MAP(level5);
IMPORT_MAP(level6);
IMPORT_MAP(level7);
IMPORT_MAP(level8);
IMPORT_MAP(hud);
IMPORT_TILES(commonTiles);


extern const UBYTE direction;
extern const UBYTE oppositeDirection;

BANKREF_EXTERN(level1Map)
extern const unsigned char level1Map[150];
BANKREF_EXTERN(level2Map)
extern const unsigned char level2Map[150];
BANKREF_EXTERN(level3Map)
extern const unsigned char level3Map[150];
BANKREF_EXTERN(level4Map)
extern const unsigned char level4Map[150];
BANKREF_EXTERN(level5Map)
extern const unsigned char level5Map[150];
BANKREF_EXTERN(level6Map)
extern const unsigned char level6Map[150];
BANKREF_EXTERN(level7Map)
extern const unsigned char level7Map[150];
BANKREF_EXTERN(level8Map)
extern const unsigned char level8Map[150];
BANKREF_EXTERN(levelDebugMap)
extern const unsigned char levelDebugMap[150];

extern uint8_t fx_00[];
extern void __mute_mask_fx_00;
extern uint8_t fx_01[];
extern void __mute_mask_fx_01;
extern uint8_t spawnTimer;

// options
BOOLEAN infiniteLives = FALSE;
BOOLEAN invincibility = FALSE;
BOOLEAN debugMode = FALSE;

//
BOOLEAN paused = FALSE;
UBYTE currentLevel = 0;
UBYTE difficultyLevel = 0;
uint16_t score = 0;
UBYTE diamonds = 0;
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
static const uint8_t goldSoundLowFreq[] = { fxC_4, fxD_4, fxE_4, fxF_4 };
static const uint8_t goldSoundHighFreq[] = { fxC_5, fxB_4, fxA_4, fxG_4 };
static uint8_t goldSoundStep = 0;
static uint8_t goldSoundGapTimer = 0;

uint8_t lastVisitedMetaCell = 0;

// handles the death state.
// sprites do not move while the active death sequence continues
uint8_t isDying = 0;
static BOOLEAN deathRespawnQueued = FALSE;
static uint16_t deathRespawnTimer = 0;

DECLARE_MUSIC(popcorn);
DECLARE_MUSIC(dirge);

// contains current game map tiles for rendering
unsigned char tileMap[736];
// contains current game map state
unsigned char levelMap[150];

struct MapInfo currentInMemoryLevel;


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

static void togglePause(void) {
	paused = !paused;
	if (paused) {
        UPDATE_HUD_TILE(1, 0, scoreFontOffset);
		UPDATE_HUD_TILE(2, 0, scoreFontOffset);
		UPDATE_HUD_TILE(3, 0, scoreFontOffset);
		UPDATE_HUD_TILE(4, 0, scoreFontOffset);
		UPDATE_HUD_TILE(5, 0, scoreFontOffset);
		UPDATE_HUD_TILE(6, 0, scoreFontOffset);
	} else {
		paintScore();
	}
}

void copyTileMapToRam(uint8_t levelToLoadBank, struct MapInfo *levelToLoad) NONBANKED {
	uint8_t __save = CURRENT_BANK;
	SWITCH_ROM(levelToLoadBank);
	// copy everything
	currentInMemoryLevel = *levelToLoad;
	// the data in the array is filled in by copyLevelMapToRam
	// redefine the pointer to my in memory array
	currentInMemoryLevel.data = tileMap;
	currentInMemoryLevel.width = 32;
	currentInMemoryLevel.height = 23;
	currentInMemoryLevel.extra_tiles_bank = BANK(commonTiles);
    currentInMemoryLevel.extra_tiles = &commonTiles;
	// if (levelToLoad->attributes) memcpy(map_attr, levelToLoad->attributes, current_level.width * current_level.height); else memset(map_attr, 0, sizeof(map_attr));
	SWITCH_ROM(__save);
}

void copyLevelMapToRam(uint8_t mapToLoadBank, const unsigned char *mapToLoad, uint8_t levelToLoadBank, struct MapInfo *levelToLoad) NONBANKED {
	uint8_t __save = CURRENT_BANK;
	SWITCH_ROM(mapToLoadBank);
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
	SWITCH_ROM(__save);
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

UBYTE tryPushBagChainFromCell(UBYTE currentCell, UBYTE direction) BANKED {
	UBYTE bagCells[mapMetaWidth];
	Sprite* activatedBags[mapMetaWidth];
	UBYTE chainCount = 0;
	UBYTE currentColumn = currentCell % mapMetaWidth;
	UBYTE bagCell;
	UBYTE destinationCell;
	UBYTE destinationValue;
	UBYTE scanColumn;
	UBYTE idx;

	if (direction == J_LEFT) {
		if (currentColumn == 0) {
			return pushBagNoBag;
		}
		bagCell = currentCell - 1;
	} else if (direction == J_RIGHT) {
		if (currentColumn == mapMetaWidth - 1) {
			return pushBagNoBag;
		}
		bagCell = currentCell + 1;
	} else {
		return pushBagNoBag;
	}

	if ((levelMap[bagCell] & metaTileBag) == 0) {
		return pushBagNoBag;
	}

	destinationCell = bagCell;
	scanColumn = bagCell % mapMetaWidth;
	while ((levelMap[destinationCell] & metaTileBag) != 0) {
		if (chainCount == mapMetaWidth) {
			return pushBagBlocked;
		}
		bagCells[chainCount++] = destinationCell;
		if (direction == J_LEFT) {
			if (scanColumn == 0) {
				return pushBagBlocked;
			}
			destinationCell--;
			scanColumn--;
		} else {
			if (scanColumn == mapMetaWidth - 1) {
				return pushBagBlocked;
			}
			destinationCell++;
			scanColumn++;
		}
	}

	destinationValue = levelMap[destinationCell];
	if ((destinationValue & (metaTileBag | metaTileEmerald | metaTileGold)) != 0) {
		return pushBagBlocked;
	}

	for (idx = 0; idx != chainCount; ++idx) {
		activatedBags[idx] = 0;
	}

	for (idx = chainCount; idx != 0; --idx) {
		Sprite* bagSprite = activateBag(bagCells[idx - 1]);
		if (bagSprite == 0) {
			while (idx < chainCount) {
				restoreStaticBag(activatedBags[idx]);
				idx++;
			}
			return pushBagBlocked;
		}
		bagSprite->custom_data[bagDirection] = direction;
		setBagState(bagSprite, statePushing);
		activatedBags[idx - 1] = bagSprite;
	}

	return pushBagStarted;
}


void updateScore(uint16_t addScore) BANKED {
	score += addScore;
	paintScore();
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

static void triggerGoldSound(void) {
	goldSoundStep = 0;
	goldSoundGapTimer = 0;
}

static void updateGoldSound(void) {
	UBYTE pairIndex;

	if (goldSoundStep >= (sizeof(goldSoundLowFreq) * 2u)) {
		return;
	}
	if (goldSoundGapTimer > 0) {
		goldSoundGapTimer--;
		return;
	}

	pairIndex = goldSoundStep >> 1;
	fx_01[fxNotePos] = (goldSoundStep & 1u) == 0 ?
		goldSoundHighFreq[pairIndex] :
		goldSoundLowFreq[pairIndex];
	ExecuteSFX(CURRENT_BANK, fx_01, SFX_MUTE_MASK(fx_01), SFX_PRIORITY_NORMAL);
	goldSoundStep++;
	goldSoundGapTimer = 3;
}

UBYTE getMapMetaTileArrayPosition(uint16_t x, uint16_t y) NONBANKED {
	const UBYTE column = LARGE_TILE_FROM_PIXEL(x - mapBoundLeft);
	const UBYTE row = LARGE_TILE_FROM_PIXEL(y - mapBoundUp);
	return row * mapMetaWidth + column;
}

BOOLEAN isMetaCellOpen(UBYTE cell) NONBANKED {
	const UBYTE column = (UBYTE)(cell % mapMetaWidth);
	const UBYTE row = (UBYTE)(cell / mapMetaWidth);
	const UBYTE tileColumn = (UBYTE)(1 + (column << 1));
	const UBYTE tileRow = (UBYTE)(2 + (row << 1));

	return checkTilesFor(tileColumn, tileRow, tileBlack);
}

void addOnMap(uint16_t x, uint16_t y, uint8_t metaTile) NONBANKED {
	const UBYTE currentCell = getMapMetaTileArrayPosition(x, y);
	levelMap[currentCell] += metaTile;
}

static void tryActivateBagAboveCell(UBYTE cellBelow) {
	UBYTE bagCell;

	if (cellBelow < mapMetaWidth) {
		return;
	}
	if (!isMetaCellOpen(cellBelow)) {
		return;
	}
	bagCell = cellBelow - mapMetaWidth;
	if ((levelMap[bagCell] & metaTileBag) == 0) {
		return;
	}
	activateBag(bagCell);
}

static void tryActivateBagsAbovePlayer(void) {
	UBYTE cells[4];
	UBYTE count = 0;
	UBYTE i;
	UBYTE cell;

	if (scroll_target == 0 || scroll_target->marked_for_removal) {
		return;
	}

	cells[count++] = getMapMetaTileArrayPosition(scroll_target->x, scroll_target->y);
	cells[count++] = getMapMetaTileArrayPosition((uint16_t)(scroll_target->x + largeTileSize - 1), scroll_target->y);
	cells[count++] = getMapMetaTileArrayPosition(scroll_target->x, (uint16_t)(scroll_target->y + largeTileSize - 1));
	cells[count++] = getMapMetaTileArrayPosition((uint16_t)(scroll_target->x + largeTileSize - 1), (uint16_t)(scroll_target->y + largeTileSize - 1));

	for (i = 0; i != count; ++i) {
		cell = cells[i];
		if ((i > 0 && cell == cells[0]) ||
			(i > 1 && cell == cells[1]) ||
			(i > 2 && cell == cells[2])) {
			continue;
		}
		tryActivateBagAboveCell(cell);
	}
}

void runMapSideEffects(void) BANKED {
	const UBYTE currentCell = getMapMetaTileArrayPosition(scroll_target->x, scroll_target->y);
	const UBYTE currentMapValue = levelMap[currentCell];
	tryActivateBagsAbovePlayer();
	if (currentCell == lastVisitedMetaCell && (currentMapValue & metaTileGold) == 0) {
		return;
	}
	if ((currentMapValue & metaTileGold) != 0) {
		const UBYTE x = TILE_FROM_PIXEL(scroll_target->x);
		const UBYTE y = TILE_FROM_PIXEL(scroll_target->y);
		updateScore(scoreGold);
		triggerGoldSound();
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
	lastVisitedMetaCell = currentCell;
}

static void resetLevelState(void) {
	uint8_t i;
	Sprite* spr;

	lastVisitedMetaCell = 0;
	deathRespawnQueued = FALSE;
	deathRespawnTimer = 0;
	if (isDying) {
		SPRITEMANAGER_ITERATE(i, spr) {
			if (!spr->marked_for_removal &&
				spr->type == SpriteBag &&
				(spr->custom_data[bagStatus] == stateShaking ||
				 spr->custom_data[bagStatus] == statePushing)) {
				restoreStaticBag(spr);
			}
		}
	}
	SpriteManagerReset();
	enemyCountOnScreen = 0;
	enemySpawned = 0;
	spawnTimer = enemyFirstSpawnTimer;
	isDying = FALSE;
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
			copyLevelMapToRam(BANK(levelDebugMap), levelDebugMap, BANK(levelDebug), &levelDebug);
			diamonds = 99;
		break;
		case 1:
			copyLevelMapToRam(BANK(level1Map), level1Map, BANK(level1), &level1);
			diamonds = 30;
		break;
		case 2: 
			copyLevelMapToRam(BANK(level2Map), level2Map, BANK(level2), &level2);
			diamonds = 41;
		break;
		case 3: 
			copyLevelMapToRam(BANK(level3Map), level3Map, BANK(level3), &level3);
			diamonds = 51;
		break;
		case 4: 
			copyLevelMapToRam(BANK(level4Map), level4Map, BANK(level4), &level4);
			diamonds = 65;
		break;
		case 5: {
			copyLevelMapToRam(BANK(level5Map), level5Map, BANK(level5), &level5);
			diamonds = 77;
		} break;
		case 6: {
			// Level tilemaps are synthesized from levelMap, so later levels can
			// reuse the same MapInfo metadata as long as the dimensions match.
			copyLevelMapToRam(BANK(level6Map), level6Map, BANK(level6), &level6);
			diamonds = 52;
		} break;
		case 7: {
			copyLevelMapToRam(BANK(level7Map), level7Map, BANK(level7), &level7);
			diamonds = 92;
		} break;
		case 8: {
			copyLevelMapToRam(BANK(level8Map), level8Map, BANK(level8), &level8);
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
	score = 0;
	currentLevel = debugMode ? 0 : 1;
	loadLevel(currentLevel);
	PlayMusic(popcorn, 1);
	INIT_HUD_EX(hud, 0, 8);
	updateScore(0);
}

void UPDATE(void) {
	if (KEY_TICKED(J_START) && !isDying) {
		togglePause();
	}
	if (paused) {
		return;
	}
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
		enemyCountOnScreen++;
		enemySpawned++;
		SpriteManagerAdd(SpriteEnemy, 232, 16);
		paintScore();
	}
	updateEmeraldSound();
	updateGoldSound();
}
