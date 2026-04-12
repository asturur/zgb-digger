#include "Banks/SetAutoBank.h"
#include "SpriteBag.h"
#include "Keys.h"
#include "SpriteManager.h"
#include "SpriteEnemy.h"
#include "StateGame.h"
#include "ZGBMain.h"
#include "Scroll.h"

const UBYTE bag_shake[] = {4, 0, 1, 0, 2};
const UBYTE bag_fall[] = {1, 3};
const UBYTE bag_static[] = {1, 0};

#define playerCrushMinHorizontalOverlap 4
#define playerCrushMinVerticalOverlap 2
// const UBYTE bag_gold_fall_start[] = {3, 6, 5, 4};

// CUSTOM_DATA usage
// 0 state
// 1 state timer
// 2 fall counter
// 3 push direction
// 4 remaining push distance
// 5 movement accumulator for future pushing

extern unsigned char levelMap[150];
extern const UBYTE direction;
BOOLEAN crushPlayerWithBag(uint16_t bagY) BANKED;
void finalizePlayerBagCrush(void) BANKED;

static BOOLEAN isPlayerTouchingBagFromBelow(void) {
    if (scroll_target == 0 || scroll_target->marked_for_removal || isDying) {
        return FALSE;
    }
    if (direction != J_UP) {
        return FALSE;
    }
    return scroll_target->x == THIS->x &&
        scroll_target->y == (uint16_t)(THIS->y + largeTileSize);
}

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

static BOOLEAN overlapsMetaSprite(UBYTE x1, UBYTE y1, UBYTE x2, UBYTE y2) {
    return x1 < (UBYTE)(x2 + largeTileSize) &&
        (UBYTE)(x1 + largeTileSize) > x2 &&
        y1 < (UBYTE)(y2 + largeTileSize) &&
        (UBYTE)(y1 + largeTileSize) > y2;
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
        isMetaCellOpen(cellBelow);
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
            bag->custom_data[bagPushOwner] = bagPushOwnerNone;
            bag->custom_data[bagFallCounter] = 0;
            bag->custom_data[bagPushDistance] = largeTileSize;
            bag->custom_data[bagMovementAccumulator] = 0;
            SetSpriteAnim(bag, bag_static, 15);
            break;
    }
}

UBYTE pushActiveBag(Sprite* bag, UBYTE direction, UBYTE owner) BANKED {
    if (bag == 0 || bag->marked_for_removal || bag->type != SpriteBag) {
        return pushBagNoBag;
    }
    if (bag->custom_data[bagStatus] == stateFalling) {
        return pushBagBlocked;
    }
    if (bag->custom_data[bagStatus] != statePushing) {
        setBagState(bag, statePushing);
        bag->custom_data[bagDirection] = direction;
        bag->custom_data[bagPushOwner] = owner;
        return pushBagStarted;
    }
    if (bag->custom_data[bagDirection] == direction) {
        if (bag->custom_data[bagPushOwner] < owner) {
            bag->custom_data[bagPushOwner] = owner;
        }
        bag->custom_data[bagPushDistance] = largeTileSize;
        return pushBagStarted;
    }
    if (owner > bag->custom_data[bagPushOwner]) {
        bag->custom_data[bagDirection] = direction;
        bag->custom_data[bagPushOwner] = owner;
        bag->custom_data[bagPushDistance] = largeTileSize;
        bag->custom_data[bagMovementAccumulator] = 0;
        return pushBagStarted;
    }
    return pushBagBlocked;
}

void restoreStaticBag(Sprite* bag) BANKED {
    const UBYTE currentCell = getMapMetaTileArrayPosition(bag->x, bag->y);
    setBagState(bag, stateStatic);
    deactivateBag(bag, (levelMap[currentCell] & tunnelMask) != 0 ? bagOnTunnel : bagOnGrass);
}

static void movePushingBag(void) {
    if (isBagAlignedToMetaCell(THIS)) {
        UBYTE pushDirection = THIS->custom_data[bagDirection];
        UBYTE pushResult = tryPushBagChainFromCell(getMapMetaTileArrayPosition(THIS->x, THIS->y), pushDirection);

        if (pushResult == pushBagBlocked) {
            THIS->custom_data[bagPushDistance] = 0;
            THIS->custom_data[bagMovementAccumulator] = 0;
            return;
        }
    }

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
        if (!spr->marked_for_removal && spr->type == SpriteEnemy && spr->y >= THIS->y) {
            if (CheckCollision(THIS, spr)) {
                crushEnemy(spr);
            }
        }
    }
}

static void scareEnemiesBelowBag(void) {
    UBYTE bagColumn;
    uint8_t i;
    Sprite* spr;

    if (THIS->x < mapBoundLeft || THIS->x > mapBoundRight) {
        return;
    }

    bagColumn = LARGE_TILE_FROM_PIXEL(THIS->x - mapBoundLeft);
    SPRITEMANAGER_ITERATE(i, spr) {
        if (!spr->marked_for_removal &&
            spr->type == SpriteEnemy &&
            spr->custom_data[enemy_direction] == J_UP &&
            spr->custom_data[mode] != deadMode &&
            spr->custom_data[mode] != waitMode &&
            spr->custom_data[mode] != crushedMode &&
            spr->y >= THIS->y &&
            LARGE_TILE_FROM_PIXEL(spr->x - mapBoundLeft) == bagColumn) {
            spr->custom_data[enemy_direction] = J_DOWN;
            spr->custom_data[movement_accumulator] = 0;
        }
    }
}

static UBYTE overlapAmount(UINT16 startA, UINT8 sizeA, UINT16 startB, UINT8 sizeB) {
    UINT16 endA = startA + sizeA;
    UINT16 endB = startB + sizeB;
    UINT16 overlapStart;
    UINT16 overlapEnd;

    if (endA <= startB || endB <= startA) {
        return 0;
    }

    overlapStart = startA > startB ? startA : startB;
    overlapEnd = endA < endB ? endA : endB;
    return (UBYTE)(overlapEnd - overlapStart);
}

static void crushPlayerUnderBag(void) {
    UBYTE horizontalOverlap;
    UBYTE verticalOverlap;

    if (scroll_target != 0 &&
        !scroll_target->marked_for_removal &&
        scroll_target->type == SpritePlayer &&
        THIS->custom_data[bagStatus] == stateFalling &&
        scroll_target->y >= THIS->y) {
        horizontalOverlap = overlapAmount(THIS->x, THIS->coll_w, scroll_target->x, scroll_target->coll_w);
        verticalOverlap = overlapAmount(THIS->y, THIS->coll_h, scroll_target->y, scroll_target->coll_h);
        if (horizontalOverlap >= playerCrushMinHorizontalOverlap &&
            verticalOverlap >= playerCrushMinVerticalOverlap) {
        crushPlayerWithBag(THIS->y);
        }
    }
}

static void finalizePushedBag(void) {
    const UBYTE currentCell = getMapMetaTileArrayPosition(THIS->x, THIS->y);
    if (bagCanFallInCellBelow(THIS)) {
        setBagState(THIS, stateFalling);
    } else {
        setBagState(THIS, stateStatic);
        deactivateBag(THIS, (levelMap[currentCell] & tunnelMask) != 0 ? bagOnTunnel : bagOnGrass);
    }
}

void START(void) {
    SetSpriteAnim(THIS, bag_static, 15);
    THIS->custom_data[bagStatus] = stateStatic;
    THIS->custom_data[bagStateTimer] = bagActivationGraceFrames;
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
    if (isDying || paused) {
        return;
    }
    if (THIS->custom_data[bagStatus] == stateStatic) {
        if (THIS->custom_data[bagStateTimer] > 0) {
            THIS->custom_data[bagStateTimer]--;
        } else if (!isPlayerTouchingBagFromBelow() && bagCanFallInCellBelow(THIS)) {
            setBagState(THIS, stateShaking);
        }
    // if is shaking and consumes the time fo shaking
    } else if (THIS->custom_data[bagStatus] == stateShaking && THIS->custom_data[bagStateTimer] > 0) {
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
        scareEnemiesBelowBag();
        if (isBagAlignedToMetaCell(THIS) == FALSE) {
            THIS->custom_data[bagFallCounter]++;
            THIS->y++;
            crushEnemiesUnderBag();
            crushPlayerUnderBag();
        } else {
            if (bagCanFallInCellBelow(THIS)) {
                uint8_t column = TILE_FROM_PIXEL(THIS->x);
                uint8_t row = 2 + TILE_FROM_PIXEL(THIS->y);
                const UBYTE currentCell = getMapMetaTileArrayPosition(THIS->x, THIS->y);
                UBYTE cellBelow = currentCell + mapMetaWidth;
                if ((levelMap[cellBelow] & metaTileGold) != 0) {
                    levelMap[cellBelow] &= (UBYTE)~metaTileGold;
                }
                levelMap[cellBelow] |= J_DOWN;
                levelMap[currentCell] |= J_UP;
                setBagTiles(column, row, tileBlack);
                THIS->custom_data[bagFallCounter]++;
                THIS->y++;
                crushEnemiesUnderBag();
                crushPlayerUnderBag();
            } else {
                // solid ground or reach end of map
                if (THIS->custom_data[bagFallCounter] >= largeTileSize * 2 && THIS->custom_data[bagStatus] == stateFalling) {
                    SpriteManagerAdd(SpriteGold, THIS->x, THIS->y);
                    finalizePlayerBagCrush();
                    SpriteManagerRemoveSprite(THIS);
                } 
                else {
                    THIS->custom_data[bagStatus] = stateStatic;
                    THIS->custom_data[bagFallCounter] = 0;
                    finalizePlayerBagCrush();
                    deactivateBag(THIS, bagOnTunnel);
                }
            }
        }
    }
}

void DESTROY(void) {
    
}
