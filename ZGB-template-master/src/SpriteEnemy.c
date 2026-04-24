#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"
#include "SpriteEnemy.h"
#include "SpriteBag.h"
#include "Scroll.h"
#include "ZGBMain.h"
#include "StateGame.h"

extern uint8_t enemyCountOnScreen;
extern UBYTE getMapMetaTileArrayPosition(uint16_t x, uint16_t y);

const UBYTE nob_walk[] = {4, 0, 1, 2, 1};
const UBYTE hob_walk[] = {4, 4, 5, 6, 5};
const UBYTE nob_dies[] = {1, 3};
const UBYTE hob_dies[] = {1, 7};

uint8_t spawnTimer = 0;

static uint8_t getEnemySpawnGapTimer(void) {
	return enemySpawnGapBaseTimer - (difficultyLevel * enemySpawnGapDifficultyStep);
}

static BOOLEAN enemyUsesHobAnimation(Sprite* enemy) {
    return enemy->custom_data[mode] == hobMode ||
        enemy->anim_data == hob_walk ||
        enemy->anim_data == hob_dies;
}

void setEnemyModeFor(Sprite* enemy, UBYTE enemyMode) BANKED {
    BOOLEAN useHobDeathAnim = enemyUsesHobAnimation(enemy);
    if (enemy->custom_data[mode] == hobMode &&
        enemyMode != hobMode &&
        enemy->custom_data[enemy_last_dig_cell] != 0xFF) {
        renderMetaCell(enemy->custom_data[enemy_last_dig_cell]);
        enemy->custom_data[enemy_last_dig_cell] = 0xFF;
    }
    enemy->custom_data[mode] = enemyMode;

    switch (enemyMode) {
        case nobMode:
            enemy->custom_data[mode_timer] = 0;
            SetSpriteAnim(enemy, nob_walk, 15);
        break;
        case hobMode:
            enemy->custom_data[mode_timer] = 0;
            SetSpriteAnim(enemy, hob_walk, 15);
        break;
        case deadMode:
            enemy->custom_data[mode_timer] = deathTimer;
            SetSpriteAnim(enemy, useHobDeathAnim ? hob_dies : nob_dies, 15);
        break;
        case waitMode:
            enemy->custom_data[mode_timer] = initialWaitTime;
        break;
        case crushedMode:
            enemy->custom_data[mode_timer] = 0;
            SetSpriteAnim(enemy, useHobDeathAnim ? hob_dies : nob_dies, 15);
        break;
        case scoreboardNobMode:
            enemy->custom_data[mode_timer] = 0;
            SetSpriteAnim(enemy, nob_walk, 15);
        break;
        case scoreboardHobMode:
            enemy->custom_data[mode_timer] = 0;
            SetSpriteAnim(enemy, hob_walk, 15);
        break;
    }
}

static void setEnemyMode(UBYTE enemyMode) {
    setEnemyModeFor(THIS, enemyMode);
}

void crushEnemy(Sprite* enemy) BANKED {
    UBYTE enemyMode = enemy->custom_data[mode];

    if (enemyMode == deadMode || enemyMode == waitMode || enemyMode == crushedMode) {
        return;
    }

    setEnemyModeFor(enemy, crushedMode);
}

UBYTE killEnemy(Sprite* enemy) BANKED {
    UBYTE enemyMode = enemy->custom_data[mode];

    if (enemyMode == deadMode || enemyMode == crushedMode) {
        return FALSE;
    }

    setEnemyModeFor(enemy, deadMode);
    return TRUE;
}

static UBYTE oppositeDirectionBit(UBYTE direction) {
    switch (direction) {
        case J_LEFT:
            return J_RIGHT;
        case J_RIGHT:
            return J_LEFT;
        case J_DOWN:
            return J_UP;
        case J_UP:
            return J_DOWN;
        default:
            return 0;
    }
}

static UBYTE absoluteAxisDistance(UBYTE a, UBYTE b) {
    return a > b ? (UBYTE)(a - b) : (UBYTE)(b - a);
}

static void demoteReverseDirection(UBYTE priorities[4], UBYTE reverseDirection) {
    UBYTE swap;

    if (reverseDirection == priorities[0]) {
        priorities[0] = priorities[1];
        priorities[1] = priorities[2];
        priorities[2] = priorities[3];
        priorities[3] = reverseDirection;
        return;
    }
    if (reverseDirection == priorities[1]) {
        priorities[1] = priorities[2];
        priorities[2] = priorities[3];
        priorities[3] = reverseDirection;
        return;
    }
    if (reverseDirection == priorities[2]) {
        swap = priorities[2];
        priorities[2] = priorities[3];
        priorities[3] = swap;
    }
}

static void invertChasePriorities(UBYTE priorities[4]) {
    UBYTE swap = priorities[0];
    priorities[0] = priorities[3];
    priorities[3] = swap;
    swap = priorities[1];
    priorities[1] = priorities[2];
    priorities[2] = swap;
}

static BOOLEAN buildEnemyChasePriorities(UBYTE priorities[4]) {
    const UBYTE currentDirection = THIS->custom_data[enemy_direction];
    const UBYTE reverseDirection = oppositeDirectionBit(currentDirection);
    Sprite* player = scroll_target;
    UBYTE towardVertical;
    UBYTE awayVertical;
    UBYTE towardHorizontal;
    UBYTE awayHorizontal;

    if (player == 0 || player->marked_for_removal) {
        return FALSE;
    }

    towardVertical = player->y < THIS->y ? J_UP : J_DOWN;
    awayVertical = towardVertical == J_UP ? J_DOWN : J_UP;
    towardHorizontal = player->x < THIS->x ? J_LEFT : J_RIGHT;
    awayHorizontal = towardHorizontal == J_LEFT ? J_RIGHT : J_LEFT;

    if (absoluteAxisDistance(player->y, THIS->y) > absoluteAxisDistance(player->x, THIS->x)) {
        priorities[0] = towardVertical;
        priorities[1] = towardHorizontal;
        priorities[2] = awayHorizontal;
        priorities[3] = awayVertical;
    } else {
        priorities[0] = towardHorizontal;
        priorities[1] = towardVertical;
        priorities[2] = awayVertical;
        priorities[3] = awayHorizontal;
    }

    if (bonusMode) {
        invertChasePriorities(priorities);
    }
    demoteReverseDirection(priorities, reverseDirection);
    return TRUE;
}

static BOOLEAN isEnemyAligned(void) {
    return MOD_FOR_LARGE_TILE(THIS->x - mapBoundLeft) == 0 && MOD_FOR_LARGE_TILE(THIS->y - mapBoundUp) == 0;
}

static BOOLEAN cellHasExit(UBYTE cell, UBYTE moveDirection) {
    const UBYTE tunnel = tunnelMap[cell];

    switch (moveDirection) {
        case J_LEFT:
            return (tunnel & tunnelHorizontalStep1) != 0;
        case J_RIGHT:
            return (tunnel & tunnelHorizontalStep4) != 0;
        case J_UP:
            return (tunnel & tunnelVerticalStep1) != 0;
        case J_DOWN:
            return (tunnel & tunnelVerticalStep4) != 0;
        default:
            return FALSE;
    }
}

static BOOLEAN cellIsFullyOpenForEntrance(UBYTE cell, UBYTE moveDirection) {
    const UBYTE tunnel = tunnelMap[cell];

    switch (moveDirection) {
        case J_LEFT:
            return (tunnel & tunnelHorizontalRightTileMask) == tunnelHorizontalRightTileMask;
        case J_RIGHT:
            return (tunnel & tunnelHorizontalLeftTileMask) == tunnelHorizontalLeftTileMask;
        case J_UP:
            return (tunnel & tunnelVerticalBottomTileMask) == tunnelVerticalBottomTileMask;
        case J_DOWN:
            return (tunnel & tunnelVerticalTopTileMask) == tunnelVerticalTopTileMask;
        default:
            return FALSE;
    }
}

static BOOLEAN overlapsMetaSprite(UBYTE x1, UBYTE y1, UBYTE x2, UBYTE y2) {
    return x1 < (UBYTE)(x2 + largeTileSize) &&
        (UBYTE)(x1 + largeTileSize) > x2 &&
        y1 < (UBYTE)(y2 + largeTileSize) &&
        (UBYTE)(y1 + largeTileSize) > y2;
}

static BOOLEAN followCrushingBag(void) {
    uint8_t i;
    Sprite* spr;

    SPRITEMANAGER_ITERATE(i, spr) {
        if (!spr->marked_for_removal &&
            spr->type == SpriteBag &&
            spr->custom_data[bagStatus] == stateFalling) {
            if (CheckCollision(THIS, spr) && THIS->y >= spr->y) {
                THIS->y = spr->y;
                return TRUE;
            }
        }
    }
    return FALSE;
}

static BOOLEAN enemyCanMove(UBYTE direction) {
    UBYTE currentCell;
    UBYTE nextCell;
    UBYTE currentColumn;
    UBYTE currentRow;

    currentCell = getMapMetaTileArrayPosition(THIS->x, THIS->y);
    currentColumn = currentCell % mapMetaWidth;
    currentRow = currentCell / mapMetaWidth;
    switch (direction) {
        case J_LEFT:
            if (THIS->x <= mapBoundLeft || currentColumn == 0) {
                return FALSE;
            }
            nextCell = currentCell - 1;
            break;
        case J_RIGHT:
            if (THIS->x >= mapBoundRight || currentColumn == mapMetaWidth - 1) {
                return FALSE;
            }
            nextCell = currentCell + 1;
            break;
        case J_UP:
            if (THIS->y <= mapBoundUp || currentRow == 0) {
                return FALSE;
            }
            nextCell = currentCell - mapMetaWidth;
            break;
        case J_DOWN:
            if (THIS->y >= mapBoundDown || currentRow == mapMetaHeight - 1) {
                return FALSE;
            }
            nextCell = currentCell + mapMetaWidth;
            break;
        default:
            return FALSE;
    }
    if (THIS->custom_data[mode] == hobMode) {
        return TRUE;
    }
    if (!cellIsFullyOpenForEntrance(nextCell, direction)) {
        return FALSE;
    }

    return cellHasExit(currentCell, direction) ||
        cellHasExit(nextCell, oppositeDirectionBit(direction));
}

static UBYTE tryPushBagAhead(void) {
    UBYTE direction = THIS->custom_data[enemy_direction];

    if (!isEnemyAligned() || THIS->custom_data[mode] != nobMode) {
        return pushBagNoBag;
    }
    if (direction != J_LEFT && direction != J_RIGHT) {
        return pushBagNoBag;
    }

    return tryPushBagChainFromCell(getMapMetaTileArrayPosition(THIS->x, THIS->y), direction);
}

static UBYTE tryPushActiveBagAhead(void) {
    UBYTE direction = THIS->custom_data[enemy_direction];
    UBYTE nextX;
    uint8_t i;
    Sprite* spr;

    if (!isEnemyAligned()) {
        return pushBagNoBag;
    }
    if (direction != J_LEFT && direction != J_RIGHT) {
        return pushBagNoBag;
    }

    nextX = direction == J_LEFT ? (UBYTE)(THIS->x - 1) : (UBYTE)(THIS->x + 1);

    SPRITEMANAGER_ITERATE(i, spr) {
        if (spr->marked_for_removal ||
            spr->type != SpriteBag ||
            spr->custom_data[bagStatus] == stateFalling) {
            continue;
        }
        if (!overlapsMetaSprite(nextX, THIS->y, spr->x, spr->y)) {
            continue;
        }
        return pushActiveBag(spr, direction, bagPushOwnerEnemy);
    }

    return pushBagNoBag;
}

static void consumeGoldAtCurrentCell(void) {
    UBYTE currentCell;

    if (!isEnemyAligned()) {
        return;
    }

    currentCell = getMapMetaTileArrayPosition(THIS->x, THIS->y);
    if (itemMap[currentCell] != itemGold) {
        return;
    }

    itemMap[currentCell] = itemNone;
    renderMetaCell(currentCell);
}

static UBYTE getEnemyLeadingDigCell(void) {
    uint16_t leadX = THIS->x;
    uint16_t leadY = THIS->y;
    const UBYTE moveDirection = THIS->custom_data[enemy_direction];

    if (moveDirection == J_RIGHT) {
        leadX = (uint16_t)(leadX + largeTileSize - 1);
    } else if (moveDirection == J_DOWN) {
        leadY = (uint16_t)(leadY + largeTileSize - 1);
    }

    return getMapMetaTileArrayPosition(leadX, leadY);
}

static UBYTE getEnemyLeadingDigSlot(void) {
    UBYTE offset;
    const UBYTE moveDirection = THIS->custom_data[enemy_direction];

    if (moveDirection == J_LEFT || moveDirection == J_RIGHT) {
        uint16_t leadX = THIS->x;
        if (moveDirection == J_RIGHT) {
            leadX = (uint16_t)(leadX + largeTileSize - 1);
        }
        offset = MOD_FOR_LARGE_TILE(leadX - mapBoundLeft);
    } else {
        uint16_t leadY = THIS->y;
        if (moveDirection == J_DOWN) {
            leadY = (uint16_t)(leadY + largeTileSize - 1);
        }
        offset = MOD_FOR_LARGE_TILE(leadY - mapBoundUp);
    }

    return offset >> 2;
}

static void finalizeEnemyDigCell(UBYTE cell) {
    if (cell != 0xFF) {
        renderMetaCell(cell);
    }
}

static void updateEnemyTunnelProgress(void) {
    const UBYTE currentDigCell = getEnemyLeadingDigCell();
    const UBYTE currentDigSlot = getEnemyLeadingDigSlot();
    const UBYTE previousDigCell = THIS->custom_data[enemy_last_dig_cell];
    const UBYTE moveDirection = THIS->custom_data[enemy_direction];

    if (previousDigCell != 0xFF && previousDigCell != currentDigCell) {
        extendTunnelProgressAt(previousDigCell, moveDirection, currentDigSlot, FALSE);
        finalizeEnemyDigCell(previousDigCell);
    }

    extendTunnelProgressAt(currentDigCell, moveDirection, currentDigSlot, TRUE);
    renderMetaCell(currentDigCell);
    THIS->custom_data[enemy_last_dig_cell] = currentDigCell;
}

static void chooseEnemyDirection(void) {
    const UBYTE currentDirection = THIS->custom_data[enemy_direction];
    const UBYTE reverseDirection = oppositeDirectionBit(currentDirection);
    UBYTE priorities[4];
    UBYTE nextDirection = currentDirection;
    UBYTE idx;

    if (!buildEnemyChasePriorities(priorities)) {
        return;
    }

    for (idx = 0; idx != 4; ++idx) {
        if (enemyCanMove(priorities[idx])) {
            nextDirection = priorities[idx];
            break;
        }
    }

    if (nextDirection != currentDirection) {
        THIS->custom_data[enemy_direction] = nextDirection;
        THIS->custom_data[enemy_movement_accumulator] = 0;
    }

    // idx == 3 means reverse was the only legal fallback, idx == 4 means blocked.
    if (THIS->custom_data[mode] == nobMode &&
        THIS->custom_data[mode_timer] < 255 &&
        (idx == 3 || idx == 4 || nextDirection == reverseDirection)) {
        THIS->custom_data[mode_timer]++;
        if (THIS->custom_data[mode_timer] > (10 - difficultyLevel)) {
            setEnemyMode(hobMode);
        }
    }
}

static BOOLEAN handleBonusPlayerCollision(void) {
    if (!bonusMode ||
        scroll_target == 0 ||
        scroll_target->marked_for_removal ||
        !CheckCollision(THIS, scroll_target)) {
        return FALSE;
    }
    if (killEnemy(THIS)) {
        scoreBonusEnemyKill();
        return TRUE;
    }
    return FALSE;
}

void START(void) {
    if(_cpu != CGB_TYPE){
        SPRITE_SET_PALETTE(THIS,1);
    }
    spawnTimer = getEnemySpawnGapTimer();
    setEnemyMode(waitMode);
    
    THIS->custom_data[enemy_direction] = J_LEFT;
    THIS->custom_data[enemy_movement_accumulator] = 20;
    THIS->custom_data[enemy_last_dig_cell] = 0xFF;
    THIS->lim_x = 256;
    THIS->lim_y = 256;
}



void UPDATE(void) {
    if (isDying || paused || THIS->custom_data[mode] == scoreboardHobMode || THIS->custom_data[mode] == scoreboardNobMode) {
        return;
    }
    if (THIS->custom_data[mode] == deadMode) {
        if (THIS->custom_data[mode_timer] > 0) {
            THIS->custom_data[mode_timer]--;
        } else {
            SpriteManagerRemoveSprite(THIS);
        }
        return;
    }
    if (THIS->custom_data[mode] == crushedMode) {
        if (followCrushingBag()) {
            return;
        }
        setEnemyMode(deadMode);
        updateScore(scoreKill);
        return;
    }
    if (THIS->custom_data[mode] == waitMode) {
        if (THIS->custom_data[mode_timer] > 0) {
            THIS->custom_data[mode_timer]--;
        } else {
            setEnemyMode(nobMode);
        }
    }

    if (THIS->custom_data[mode] != nobMode && THIS->custom_data[mode] != hobMode) {
        return;
    }
    if (THIS->custom_data[mode] == hobMode && THIS->custom_data[mode_timer] < 255) {
        THIS->custom_data[mode_timer]++;
    }
    if (isEnemyAligned()) {
        UBYTE activePushResult;

        if (THIS->custom_data[mode] == hobMode && THIS->custom_data[mode_timer] > (30 + (difficultyLevel << 1))) {
            setEnemyMode(nobMode);
        }
        chooseEnemyDirection();
        activePushResult = tryPushActiveBagAhead();
        if (activePushResult == pushBagBlocked) {
            THIS->custom_data[enemy_direction] = oppositeDirectionBit(THIS->custom_data[enemy_direction]);
            THIS->custom_data[enemy_movement_accumulator] = 0;
            return;
        }
        if (activePushResult == pushBagNoBag && tryPushBagAhead() == pushBagBlocked) {
            THIS->custom_data[enemy_direction] = oppositeDirectionBit(THIS->custom_data[enemy_direction]);
            THIS->custom_data[enemy_movement_accumulator] = 0;
            return;
        }
        consumeGoldAtCurrentCell();
    }
    THIS->custom_data[enemy_movement_accumulator] += 4;
    if (THIS->custom_data[enemy_movement_accumulator] < 25) {
        return;
    }
    THIS->custom_data[enemy_movement_accumulator] -= 5;
    switch (THIS->custom_data[enemy_direction]) {
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
        case J_UP:
            if (THIS->y > mapBoundUp) {
                THIS->y--;
            }
        break;
        case J_DOWN:
            if (THIS->y < mapBoundDown) {
                THIS->y++;
            }
        break;
    }
    if (handleBonusPlayerCollision()) {
        return;
    }
    if (THIS->custom_data[mode] == hobMode) {
        updateEnemyTunnelProgress();
    } else if (THIS->custom_data[enemy_last_dig_cell] != 0xFF) {
        finalizeEnemyDigCell(THIS->custom_data[enemy_last_dig_cell]);
        THIS->custom_data[enemy_last_dig_cell] = 0xFF;
    }
}

void DESTROY(void) {
    enemyCountOnScreen--;
    if (spawnTimer == 0) {
        spawnTimer = getEnemySpawnGapTimer();
    }
}
