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

#define META_CELL_TILE_COLUMN(CELL) ((UBYTE)(1 + (((CELL) % mapMetaWidth) << 1)))
#define META_CELL_TILE_ROW(CELL) ((UBYTE)(2 + (((CELL) / mapMetaWidth) << 1)))

// contains current game map tiles for rendering
unsigned char tileMap[736];
unsigned char itemMap[150];
unsigned char tunnelMap[150];

struct MapInfo currentInMemoryLevel;

static UBYTE countItemsOnMap(UBYTE item) {
	UBYTE cell;
	UBYTE count = 0;

	for (cell = 0; cell != 150; ++cell) {
		if (itemMap[cell] == item) {
			count++;
		}
	}

	return count;
}

static UBYTE legacySeedToTunnel(UBYTE seed) NONBANKED {
	UBYTE tunnel = 0;

	if ((seed & legacySeedTunnelLeft) != 0) {
		tunnel |= tunnelHorizontalStep12;
	}
	if ((seed & legacySeedTunnelRight) != 0) {
		tunnel |= tunnelHorizontalStep34;
	}
	if ((seed & legacySeedTunnelUp) != 0) {
		tunnel |= tunnelVerticalStep12;
	}
	if ((seed & legacySeedTunnelDown) != 0) {
		tunnel |= tunnelVerticalStep34;
	}

	if ((tunnel & tunnelHorizontalCenterMask) == tunnelHorizontalCenterMask) {
		tunnel |= tunnelVerticalCenterMask;
	}
	if ((tunnel & tunnelVerticalCenterMask) == tunnelVerticalCenterMask) {
		tunnel |= tunnelHorizontalCenterMask;
	}

	return tunnel;
}

void determineDigTiles(
	UBYTE currentCell,
	UBYTE topCell,
	UBYTE rightCell,
	UBYTE bottomCell,
	UBYTE leftCell,
	UBYTE* tiles
) NONBANKED {
	(void)topCell;
	(void)rightCell;
	(void)bottomCell;
	(void)leftCell;

	tiles[0] = tileGrass;
	tiles[1] = tileGrass;
	tiles[2] = tileGrass;
	tiles[3] = tileGrass;

	switch (currentCell) {
		// Solid grass cell with no tunnel opened yet.
		case 0x00:
			break;

		// HORIZONTAL
		// Horizontal cell being dug from the left, first quarter opened.
		case tunnelHorizontalStep1:
			tiles[0] = tileHalfDigLeftTop;
			tiles[2] = tileHalfDigLeftBottom;
			break;

		// Horizontal cell open on the left tile only, or left horizontal dead-end.
		case tunnelHorizontalStep12:
			tiles[0] = tileTopRightWall;
			tiles[2] = tileBottomRightWall;
			break;

		// Horizontal cell being dug from the left into the right tile.
		case (tunnelVerticalCenterMask | tunnelHorizontalStep123 ):
			tiles[0] = tileTopWall;
			tiles[2] = tileBottomWall;
			tiles[1] = tileHalfDigLeftTop;
			tiles[3] = tileHalfDigLeftBottom;
			break;

		// Fully dug horizontal cell.
		case (tunnelVerticalCenterMask | tunnelHorizontalMask):
			tiles[0] = tileTopWall;
			tiles[2] = tileBottomWall;
			tiles[1] = tileTopWall;
			tiles[3] = tileBottomWall;
			break;

		// Horizontal cell being dug from the right, first quarter opened.
		case tunnelHorizontalStep4:
			tiles[1] = tileHalfDigRightTop;
			tiles[3] = tileHalfDigRightBottom;
			break;

		// Horizontal cell open on the right tile only, or right horizontal dead-end.
		case tunnelHorizontalStep34:
			tiles[1] = tileTopLeftWall;
			tiles[3] = tileBottomLeftWall;
			break;

		// Horizontal cell being dug from the right into the left tile.
		case (tunnelVerticalCenterMask | tunnelHorizontalStep234):
			tiles[0] = tileHalfDigRightTop;
			tiles[2] = tileHalfDigRightBottom;
			tiles[1] = tileTopWall;
			tiles[3] = tileBottomWall;
			break;

		// VERTICAL
		// Vertical cell being dug from the top, first quarter opened.
		case tunnelVerticalStep1:
			tiles[0] = tileHalfDigTopLeft;
			tiles[1] = tileHalfDigTopRight;
			break;

		// Vertical cell open on the top tile only, or top vertical dead-end.
		case tunnelVerticalStep12:
			tiles[0] = tileBottomLeftWall;
			tiles[1] = tileBottomRightWall;
			break;

		// Vertical cell being dug from the top into the bottom tile.
		case (tunnelHorizontalCenterMask | tunnelVerticalStep123):
			tiles[0] = tileLeftWall;
			tiles[1] = tileRightWall;
			tiles[2] = tileHalfDigTopLeft;
			tiles[3] = tileHalfDigTopRight;
			break;

		// Fully dug vertical cell.
		case (tunnelHorizontalCenterMask | tunnelVerticalMask):
			tiles[0] = tileLeftWall;
			tiles[1] = tileRightWall;
			tiles[2] = tileLeftWall;
			tiles[3] = tileRightWall;
			break;

		// Vertical cell being dug from the bottom, first quarter opened.
		case tunnelVerticalStep4:
			tiles[2] = tileHalfDigBottomLeft;
			tiles[3] = tileHalfDigBottomRight;
			break;

		// Vertical cell open on the bottom tile only, or bottom vertical dead-end.
		case tunnelVerticalStep34:
			tiles[2] = tileTopLeftWall;
			tiles[3] = tileTopRightWall;
			break;

		// Vertical cell being dug from the bottom into the top tile.
		case (tunnelHorizontalCenterMask | tunnelVerticalStep234):
			tiles[0] = tileHalfDigBottomLeft;
			tiles[1] = tileHalfDigBottomRight;
		    tiles[2] = tileLeftWall;
			tiles[3] = tileRightWall;
			break;

		// CORNERS
		// Corner cell open to the left and up.
		case ( tunnelHorizontalStep12 | tunnelVerticalStep12 ):
		    tiles[0] = tileBlack;
			tiles[1] = tileRightWall;
		    tiles[2] = tileBottomWall;
			tiles[3] = tileBottomRightWall;
		    break;

		// Corner cell open to the right and up.
		case ( tunnelHorizontalStep34 | tunnelVerticalStep12 ):
		    tiles[0] = tileLeftWall;
			tiles[1] = tileBlack;
		    tiles[2] = tileBottomLeftWall;
			tiles[3] = tileBottomWall;
		    break;

		// Corner cell open to the right and down.
		case ( tunnelHorizontalStep34 | tunnelVerticalStep34 ):
		    tiles[0] = tileTopLeftWall;
			tiles[1] = tileTopWall;
		    tiles[2] = tileLeftWall;
			tiles[3] = tileBlack;
		    break;

		// Corner cell open to the left and down.
		case ( tunnelHorizontalStep12 | tunnelVerticalStep34 ):
		    tiles[0] = tileTopWall;
			tiles[1] = tileTopRightWall;
		    tiles[2] = tileBlack;
			tiles[3] = tileRightWall;
		    break;

		// T JUNCTIONS
		// Tee cell open left, right, and up.
		case (tunnelHorizontalMask | tunnelVerticalStep123):
			tiles[0] = tileBlack;
			tiles[1] = tileBlack;
		    tiles[2] = tileBottomWall;
			tiles[3] = tileBottomWall;
			break;

		// Tee cell open left, right, and down.
		case (tunnelHorizontalMask | tunnelVerticalStep234):
			tiles[0] = tileTopWall;
			tiles[1] = tileTopWall;
		    tiles[2] = tileBlack;
			tiles[3] = tileBlack;
			break;

		// Tee cell open up, down, and left.
		case (tunnelVerticalMask | tunnelHorizontalStep123):
			tiles[0] = tileBlack;
			tiles[1] = tileRightWall;
		    tiles[2] = tileBlack;
			tiles[3] = tileRightWall;
			break;
		
	    // Tee cell open up, down, and right.
		case (tunnelVerticalMask | tunnelHorizontalStep234):
			tiles[0] = tileLeftWall;
			tiles[1] = tileBlack;
		    tiles[2] = tileLeftWall;
			tiles[3] = tileBlack;
			break;

		// Cross cell open in all four directions.
		case tunnelVerticalMask | tunnelHorizontalMask:
		    tiles[0] = tileBlack;
			tiles[1] = tileBlack;
		    tiles[2] = tileBlack;
			tiles[3] = tileBlack;
		    break;

		// BAD DIGGED STUFF
		// Digged one  bottom, one right
		case (tunnelVerticalStep4 | tunnelHorizontalStep4):
		    tiles[1] = tileHalfDigRightTop;
			tiles[2] = tileHalfDigBottomLeft;
			tiles[3] = tileDig75TopLeft;
            break;
		// Digged one  bottom, one left
		case (tunnelVerticalStep4 | tunnelHorizontalStep1):
		    tiles[0] = tileHalfDigLeftTop;
			tiles[2] = tileDig75TopRight;
			tiles[3] = tileHalfDigBottomRight;
            break;
		// Digged one top, one left
		case (tunnelVerticalStep1 | tunnelHorizontalStep1):
		    tiles[0] = tileDig75BottomRight;
			tiles[1] = tileHalfDigTopRight;
			tiles[2] = tileHalfDigLeftBottom;
            break;
		// Digged one top, one right
		case (tunnelVerticalStep1 | tunnelHorizontalStep4):
		    tiles[0] = tileHalfDigTopLeft;
			tiles[1] = tileDig75BottomLeft;
			tiles[3] = tileHalfDigRightBottom;
            break;
		// Any other byte is currently treated as unknown or invalid tunnel topology.
		default:
			break;
	}
}

void updateVideoMemAndMap(UBYTE column, UBYTE row, UBYTE type) NONBANKED {
	set_bkg_tile_xy(column, row, type);
    tileMap[row * tilesPerRow + column] = type;
}

void renderMetaCell(UBYTE cell) BANKED {
	UBYTE tiles[4];
	const UBYTE tileColumn = META_CELL_TILE_COLUMN(cell);
	const UBYTE tileRow = META_CELL_TILE_ROW(cell);
	const UBYTE item = itemMap[cell];
	const UBYTE tunnel = tunnelMap[cell];

	if (item == itemEmerald) {
		tiles[0] = tileEmeraldTL;
		tiles[1] = tileEmeraldTR;
		tiles[2] = tileEmeraldBL;
		tiles[3] = tileEmeraldBR;
	} else if (item == itemBag) {
		if (tunnel != 0) {
			tiles[0] = bagTL;
			tiles[1] = bagTR;
			tiles[2] = bagBL;
			tiles[3] = bagBR;
		} else {
			tiles[0] = tileBagTL;
			tiles[1] = tileBagTR;
			tiles[2] = tileBagBL;
			tiles[3] = tileBagBR;
		}
	} else if (item == itemGold || item == itemBonus) {
		tiles[0] = goldTL;
		tiles[1] = goldTR;
		tiles[2] = goldBL;
		tiles[3] = goldBR;
	} else if (tunnel == 0) {
		tiles[0] = tileGrass;
		tiles[1] = tileGrass;
		tiles[2] = tileGrass;
		tiles[3] = tileGrass;
	} else {
		determineDigTiles(tunnel, 0, 0, 0, 0, tiles);
	}

	updateVideoMemAndMap(tileColumn, tileRow, tiles[0]);
	updateVideoMemAndMap(tileColumn + 1, tileRow, tiles[1]);
	updateVideoMemAndMap(tileColumn, tileRow + 1, tiles[2]);
	updateVideoMemAndMap(tileColumn + 1, tileRow + 1, tiles[3]);
}

static BOOLEAN tryGetAdjacentCell(UBYTE cell, UBYTE moveDirection, UBYTE* adjacentCell) NONBANKED {
	const UBYTE column = (UBYTE)(cell % mapMetaWidth);
	const UBYTE row = (UBYTE)(cell / mapMetaWidth);

	switch (moveDirection) {
		case J_LEFT:
			if (column == 0) {
				return FALSE;
			}
			*adjacentCell = cell - 1;
			return TRUE;
		case J_RIGHT:
			if (column == mapMetaWidth - 1) {
				return FALSE;
			}
			*adjacentCell = cell + 1;
			return TRUE;
		case J_UP:
			if (row == 0) {
				return FALSE;
			}
			*adjacentCell = cell - mapMetaWidth;
			return TRUE;
		case J_DOWN:
			if (row == mapMetaHeight - 1) {
				return FALSE;
			}
			*adjacentCell = cell + mapMetaWidth;
			return TRUE;
		default:
			return FALSE;
	}
}

void openTunnelConnection(UBYTE fromCell, UBYTE moveDirection) NONBANKED {
	UBYTE adjacentCell;

	if (!tryGetAdjacentCell(fromCell, moveDirection, &adjacentCell)) {
		return;
	}

		switch (moveDirection) {
			case J_LEFT:
				tunnelMap[fromCell] |= tunnelHorizontalStep12;
				tunnelMap[adjacentCell] |= tunnelHorizontalStep34;
				break;
			case J_RIGHT:
				tunnelMap[fromCell] |= tunnelHorizontalStep34;
				tunnelMap[adjacentCell] |= tunnelHorizontalStep12;
				break;
			case J_UP:
				tunnelMap[fromCell] |= tunnelVerticalStep12;
				tunnelMap[adjacentCell] |= tunnelVerticalStep34;
				break;
			case J_DOWN:
				tunnelMap[fromCell] |= tunnelVerticalStep34;
				tunnelMap[adjacentCell] |= tunnelVerticalStep12;
				break;
			default:
				return;
		}

		if ((tunnelMap[fromCell] & tunnelHorizontalCenterMask) == tunnelHorizontalCenterMask) {
			tunnelMap[fromCell] |= tunnelVerticalCenterMask;
		}
		if ((tunnelMap[fromCell] & tunnelVerticalCenterMask) == tunnelVerticalCenterMask) {
			tunnelMap[fromCell] |= tunnelHorizontalCenterMask;
		}
		if ((tunnelMap[adjacentCell] & tunnelHorizontalCenterMask) == tunnelHorizontalCenterMask) {
			tunnelMap[adjacentCell] |= tunnelVerticalCenterMask;
		}
		if ((tunnelMap[adjacentCell] & tunnelVerticalCenterMask) == tunnelVerticalCenterMask) {
			tunnelMap[adjacentCell] |= tunnelHorizontalCenterMask;
		}

	renderMetaCell(fromCell);
	renderMetaCell(adjacentCell);
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
	UBYTE cell;

	SWITCH_ROM(mapToLoadBank);
	memset(itemMap, itemNone, sizeof(itemMap));
	memset(tunnelMap, 0, sizeof(tunnelMap));
	// fill up the first lines of the map with 2 lines 0s and 1 of 1s
	memset(tileMap, 0, hudSize);
	memset(tileMap + hudSize, 1, tilesPerRow * 22);

	for (cell = 0; cell != 150; ++cell) {
		UBYTE renderedTiles[4];
		const UBYTE seed = mapToLoad[cell];
		const UBYTE tileColumn = META_CELL_TILE_COLUMN(cell);
		const UBYTE tileRow = META_CELL_TILE_ROW(cell);

		tunnelMap[cell] = legacySeedToTunnel(seed);
		if (seed == seedItemEmerald) {
			itemMap[cell] = itemEmerald;
		} else if (seed == seedItemBag) {
			itemMap[cell] = itemBag;
		}

		if (itemMap[cell] == itemEmerald) {
			renderedTiles[0] = tileEmeraldTL;
			renderedTiles[1] = tileEmeraldTR;
			renderedTiles[2] = tileEmeraldBL;
			renderedTiles[3] = tileEmeraldBR;
		} else if (itemMap[cell] == itemBag) {
			if (tunnelMap[cell] != 0) {
				renderedTiles[0] = bagTL;
				renderedTiles[1] = bagTR;
				renderedTiles[2] = bagBL;
				renderedTiles[3] = bagBR;
			} else {
				renderedTiles[0] = tileBagTL;
				renderedTiles[1] = tileBagTR;
				renderedTiles[2] = tileBagBL;
				renderedTiles[3] = tileBagBR;
			}
		} else if (itemMap[cell] == itemGold || itemMap[cell] == itemBonus) {
			renderedTiles[0] = goldTL;
			renderedTiles[1] = goldTR;
			renderedTiles[2] = goldBL;
			renderedTiles[3] = goldBR;
		} else if (tunnelMap[cell] == 0) {
			renderedTiles[0] = tileGrass;
			renderedTiles[1] = tileGrass;
			renderedTiles[2] = tileGrass;
			renderedTiles[3] = tileGrass;
		} else {
			determineDigTiles(tunnelMap[cell], 0, 0, 0, 0, renderedTiles);
		}
        uint16_t tileNum = tileRow * tilesPerRow + tileColumn;
		tileMap[tileNum] = renderedTiles[0];
		tileMap[tileNum + 1] = renderedTiles[1];
		tileMap[tileNum + tilesPerRow] = renderedTiles[2];
		tileMap[tileNum + tilesPerRow + 1] = renderedTiles[3];
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
		itemMap[bagcell] = itemNone;
		renderMetaCell(bagcell);
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

	if (itemMap[bagCell] != itemBag) {
		return pushBagNoBag;
	}

	destinationCell = bagCell;
	scanColumn = bagCell % mapMetaWidth;
	while (itemMap[destinationCell] == itemBag) {
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

	destinationValue = itemMap[destinationCell];
	if (destinationValue != itemNone) {
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

static void tryActivateBagAboveCell(UBYTE cellBelow) {
	UBYTE bagCell;

	if (cellBelow < mapMetaWidth) {
		return;
	}
	if (tunnelMap[cellBelow] == 0) {
		return;
	}
	bagCell = cellBelow - mapMetaWidth;
	if (itemMap[bagCell] != itemBag) {
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

static UBYTE getPlayerLeadingMetaCell(void) {
	uint16_t leadX = scroll_target->x;
	uint16_t leadY = scroll_target->y;

	if (direction == J_RIGHT) {
		leadX = (uint16_t)(leadX + largeTileSize - 1);
	} else if (direction == J_DOWN) {
		leadY = (uint16_t)(leadY + largeTileSize - 1);
	}

	return getMapMetaTileArrayPosition(leadX, leadY);
}

void runMapSideEffects(void) BANKED {
	const UBYTE currentCell = getPlayerLeadingMetaCell();
	const UBYTE currentItem = itemMap[currentCell];
	tryActivateBagsAbovePlayer();
	if (currentCell == lastVisitedMetaCell && currentItem != itemGold) {
		return;
	}
	if (currentItem == itemGold) {
		updateScore(scoreGold);
		triggerGoldSound();
		itemMap[currentCell] = itemNone;
		renderMetaCell(currentCell);
	}
	// we eat a gem
	if (currentItem == itemEmerald) {
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
		itemMap[currentCell] = itemNone;
		renderMetaCell(currentCell);
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
			diamonds = countItemsOnMap(itemEmerald);
		break;
		case 2: 
			copyLevelMapToRam(BANK(level2Map), level2Map, BANK(level2), &level2);
			diamonds = countItemsOnMap(itemEmerald);
		break;
		case 3: 
			copyLevelMapToRam(BANK(level3Map), level3Map, BANK(level3), &level3);
			diamonds = countItemsOnMap(itemEmerald);
		break;
		case 4: 
			copyLevelMapToRam(BANK(level4Map), level4Map, BANK(level4), &level4);
			diamonds = countItemsOnMap(itemEmerald);
		break;
		case 5: {
			copyLevelMapToRam(BANK(level5Map), level5Map, BANK(level5), &level5);
			diamonds = countItemsOnMap(itemEmerald);
		} break;
		case 6: {
			// Level tilemaps are synthesized from the split runtime maps, so later levels can
			// reuse the same MapInfo metadata as long as the dimensions match.
			copyLevelMapToRam(BANK(level6Map), level6Map, BANK(level6), &level6);
			diamonds = countItemsOnMap(itemEmerald);
		} break;
		case 7: {
			copyLevelMapToRam(BANK(level7Map), level7Map, BANK(level7), &level7);
			diamonds = countItemsOnMap(itemEmerald);
		} break;
		case 8: {
			copyLevelMapToRam(BANK(level8Map), level8Map, BANK(level8), &level8);
			diamonds = countItemsOnMap(itemEmerald);
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
	OBP0_REG = DMG_PALETTE(DMG_WHITE, DMG_LITE_GRAY, DMG_DARK_GRAY, DMG_BLACK); // normal palette
	OBP1_REG = DMG_PALETTE(DMG_BLACK, DMG_WHITE, DMG_LITE_GRAY, DMG_DARK_GRAY); // bright palette
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
