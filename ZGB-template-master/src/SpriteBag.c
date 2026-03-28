#include "Banks/SetAutoBank.h"
#include "SpriteBag.h"
#include "Keys.h"
#include "SpriteManager.h"
#include "SpriteEnemy.h"
#include "StateGame.h"
#include "ZGBMain.h"

const UBYTE bag_shake[] = {4, 0, 1, 0, 2};
const UBYTE bag_fall[] = {1, 3};
const UBYTE bag_static[] = {1, 0};
// const UBYTE bag_gold_fall_start[] = {3, 6, 5, 4};

// CUSTOM_DATA usage
// 0 state
// 1 state timer
// 2 fall counter
// 3 push direction
// 4 remaining push distance
// 5 movement accumulator for future pushing

extern unsigned char levelMap[150];

static void setBagTiles(UBYTE column, UBYTE row, UBYTE type) {
    updateVideoMemAndMap(column, row, type);
    updateVideoMemAndMap(column + 1, row, type);
    updateVideoMemAndMap(column, row + 1, type);
    updateVideoMemAndMap(column + 1, row + 1, type);
}

static void updateBagTiles(uint8_t tileType) {
    UBYTE bagColumn = TILE_FROM_PIXEL(THIS->x);
    UBYTE bagRow = TILE_FROM_PIXEL(THIS->y);
    setBagTiles(bagColumn, bagRow, tileType);
}

static void deactivateBag(Sprite* bag, BOOLEAN onBlack) {
	uint8_t column = TILE_FROM_PIXEL(bag->x);
	uint8_t row = TILE_FROM_PIXEL(bag->y);
	updateVideoMemAndMap(column, row, onBlack ? bagTL : tileBagTL);
	updateVideoMemAndMap(column + 1, row, onBlack ? bagTR : tileBagTR);
	updateVideoMemAndMap(column, row + 1, onBlack ? bagBL : tileBagBL);
	updateVideoMemAndMap(column + 1, row + 1, onBlack ? bagBR : tileBagBR);
	addOnMap(bag->x, bag->y, metaTileBag);
	SpriteManagerRemoveSprite(bag);
}

static BOOLEAN isBagAlignedToMetaCell(Sprite* bag) {
    return MOD_FOR_LARGE_TILE(bag->x - mapBoundLeft) == 0 &&
        MOD_FOR_LARGE_TILE(bag->y - mapBoundUp) == 0;
}

static BOOLEAN bagCanFallInCellBelow(Sprite* bag) {
    UBYTE cellBelow;
    if (bag->y >= mapBoundDown) {
        return FALSE;
    }
    if (LARGE_TILE_FROM_PIXEL(bag->y - mapBoundUp) >= mapMetaHeight - 1) {
        return FALSE;
    }
    cellBelow = getMapMetaTileArrayPosition(bag->x, bag->y) + mapMetaWidth;
    return (levelMap[cellBelow] & metaTileBag) == 0 &&
        (levelMap[cellBelow] & tunnelMask) != 0;
}

void setBagState(Sprite* bag, UBYTE bagState) BANKED {
    bag->custom_data[bagStatus] = bagState;

    switch (bagState) {
        case stateStatic:
            bag->custom_data[bagStateTimer] = 0;
            bag->custom_data[bagFallCounter] = 0;
            SetSpriteAnim(bag, bag_static, 15);
            break;
        case stateShaking:
            bag->custom_data[bagStateTimer] = shakeBeforeFall;
            bag->custom_data[bagFallCounter] = 0;
            SetSpriteAnim(bag, bag_shake, 15);
            break;
        case stateFalling:
            bag->custom_data[bagStateTimer] = 0;
            bag->custom_data[bagFallCounter] = 0;
            SetSpriteAnim(bag, bag_fall, 15);
            break;
        case statePushing:
            bag->custom_data[bagStateTimer] = largeTileSize;
            bag->custom_data[bagFallCounter] = 0;
            bag->custom_data[bagPushDistance] = largeTileSize;
            bag->custom_data[bagMovementAccumulator] = 0;
            SetSpriteAnim(bag, bag_static, 15);
            break;
    }
}

void restoreStaticBag(Sprite* bag) BANKED {
    UBYTE currentColumn = LARGE_TILE_FROM_PIXEL(bag->x - mapBoundLeft);
    UBYTE currentRow = LARGE_TILE_FROM_PIXEL(bag->y - mapBoundUp);
    UBYTE currentCell = currentRow * mapMetaWidth + currentColumn;

    setBagState(bag, stateStatic);
    deactivateBag(bag, (levelMap[currentCell] & tunnelMask) != 0 ? bagOnTunnel : bagOnGrass);
}

static void movePushingBag(void) {
    THIS->custom_data[bagMovementAccumulator] += 4;
    if (THIS->custom_data[bagMovementAccumulator] < 5) {
        return;
    }
    THIS->custom_data[bagMovementAccumulator] -= 5;

    switch (THIS->custom_data[bagDirection]) {
        case J_LEFT:
            if (THIS->x > mapBoundLeft) {
                THIS->x--;
            }
            break;
        case J_RIGHT:
            if (THIS->x < mapBoundRight) {
                THIS->x++;
            }
            break;
    }

    if (THIS->custom_data[bagPushDistance] > 0) {
        THIS->custom_data[bagPushDistance]--;
    }
}

static void crushEnemiesUnderBag(void) {
    uint8_t i;
    Sprite* spr;

    SPRITEMANAGER_ITERATE(i, spr) {
        if (spr->type == SpriteEnemy && spr->y >= THIS->y) {
            if (CheckCollision(THIS, spr)) {
                crushEnemy(spr);
            }
        }
    }
}

static void finalizePushedBag(void) {
    UBYTE currentColumn = LARGE_TILE_FROM_PIXEL(THIS->x - mapBoundLeft);
    UBYTE currentRow = LARGE_TILE_FROM_PIXEL(THIS->y - mapBoundUp);
    UBYTE currentCell = currentRow * mapMetaWidth + currentColumn;

    if (bagCanFallInCellBelow(THIS)) {
        setBagState(THIS, stateFalling);
    } else {
        setBagState(THIS, stateStatic);
        deactivateBag(THIS, (levelMap[currentCell] & tunnelMask) != 0 ? bagOnTunnel : bagOnGrass);
    }
}

void START(void) {
    SetSpriteAnim(THIS, bag_shake, 15);
    THIS->custom_data[bagStatus] = stateShaking;
    THIS->custom_data[bagStateTimer] = shakeBeforeFall;
    THIS->custom_data[bagFallCounter] = 0;
    THIS->custom_data[bagDirection] = 0;
    THIS->custom_data[bagPushDistance] = 0;
    THIS->custom_data[bagMovementAccumulator] = 0;
    THIS->lim_x = 256;
    THIS->lim_y = 256;
    UBYTE bagColumn = TILE_FROM_PIXEL(THIS->x);
    UBYTE bagRow = TILE_FROM_PIXEL(THIS->y);
    if (getTileMapTile(bagColumn, bagRow) >= bagTL) {
        updateBagTiles(tileBlack);
    } else {
        updateBagTiles(tileGrass);
    }
}

void UPDATE(void) {
    // if is shaking and consumes the time fo shaking
    if (THIS->custom_data[bagStatus] == stateShaking && THIS->custom_data[bagStateTimer] > 0) {
         THIS->custom_data[bagStateTimer]--;
    // it starts to fall down
    } else if (THIS->custom_data[bagStatus] == stateShaking && THIS->custom_data[bagStateTimer] == 0) {
        setBagState(THIS, stateFalling);
    } else if (THIS->custom_data[bagStatus] == statePushing) {
        movePushingBag();
        if (THIS->custom_data[bagPushDistance] == 0 && isBagAlignedToMetaCell(THIS)) {
            finalizePushedBag();
        }
    // else if is falling down as a bag or as a pile of gold
    } else if (THIS->custom_data[bagStatus] == stateFalling && THIS->y <= mapBoundDown) {
        if (MOD_FOR_LARGE_TILE(THIS->y)) {
            THIS->custom_data[bagFallCounter]++;
            THIS->y++;
            crushEnemiesUnderBag();
        } else {
            UBYTE cellBelow = getMapMetaTileArrayPosition(THIS->x, THIS->y) + mapMetaWidth;
            uint8_t column = TILE_FROM_PIXEL(THIS->x);
            uint8_t row = 2 + TILE_FROM_PIXEL(THIS->y);
            if (bagCanFallInCellBelow(THIS)) {
                if ((levelMap[cellBelow] & metaTileGold) != 0) {
                    levelMap[cellBelow] &= (UBYTE)~metaTileGold;
                }
                setBagTiles(column, row, tileBlack);
                THIS->custom_data[bagFallCounter]++;
                THIS->y++;
                crushEnemiesUnderBag();
            } else {
                // solid ground or reach end of map
                if (THIS->custom_data[bagFallCounter] >= largeTileSize && THIS->custom_data[bagStatus] == stateFalling) {
                    SpriteManagerAdd(SpriteGold, THIS->x, THIS->y);
                    SpriteManagerRemoveSprite(THIS);
                } 
                else {
                    THIS->custom_data[bagStatus] = stateStatic;
                    THIS->custom_data[bagFallCounter] = 0;
                    deactivateBag(THIS, bagOnTunnel);
                }
            }
        }
    }
}

void DESTROY(void) {
    
}
