#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"
#include "SpriteEnemy.h"
#include "StateGame.h"

extern unsigned char levelMap[150];
extern uint8_t enemyCountOnScreen;
extern uint8_t spawnTimer;
extern uint8_t isDying;
extern UBYTE getMapMetaTileArrayPosition(uint16_t x, uint16_t y);

const UBYTE nob_walk[] = {4, 0, 1, 2, 1};
const UBYTE hob_walk[] = {4, 4, 5, 6, 5};
const UBYTE nob_dies[] = {1, 3};
const UBYTE hob_dies[] = {1, 7};

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

static BOOLEAN isEnemyAligned(void) {
    return MOD_FOR_LARGE_TILE(THIS->x - mapBoundLeft) == 0 && MOD_FOR_LARGE_TILE(THIS->y - mapBoundUp) == 0;
}

static UBYTE getEnemyCurrentTunnel(void) {

    UBYTE currentCell;

    if (THIS->x < mapBoundLeft || THIS->y < mapBoundUp || THIS->x > mapBoundRight || THIS->y > mapBoundDown) {
        return 0;
    }

    currentCell = getMapMetaTileArrayPosition(THIS->x, THIS->y);
	return levelMap[currentCell];
}

static BOOLEAN enemyCanMove(UBYTE direction, UBYTE tunnel) {
    return (tunnel & oppositeDirectionBit(direction)) != 0;
}

static void chooseEnemyDirection(void) {
    const UBYTE tunnel = getEnemyCurrentTunnel();
    const UBYTE currentDirection = THIS->custom_data[enemy_direction];
    const UBYTE reverseDirection = oppositeDirectionBit(currentDirection);
    UBYTE priorities[3];
    UBYTE idx;

    if (enemyCanMove(currentDirection, tunnel)) {
        return;
    }

    if (currentDirection == J_LEFT || currentDirection == J_RIGHT) {
        priorities[0] = J_DOWN;
        priorities[1] = J_UP;
    } else {
        priorities[0] = J_LEFT;
        priorities[1] = J_RIGHT;
    }
    priorities[2] = reverseDirection;

    for (idx = 0; idx != 3; ++idx) {
        if (enemyCanMove(priorities[idx], tunnel)) {
            THIS->custom_data[enemy_direction] = priorities[idx];
            THIS->custom_data[movement_accumulator] = 0;
            return;
        }
    }
}

void START(void) {
    SetSpriteAnim(THIS, nob_walk, 15);
    THIS->custom_data[hobOrNobOrDead] = waitMode;
    THIS->custom_data[timer] = initialWaitTime;
    THIS->custom_data[enemy_direction] = J_LEFT;
    THIS->custom_data[movement_accumulator] = 20;
    THIS->lim_x = 256;
    THIS->lim_y = 256;
}



void UPDATE(void) {
    if (isDying == 1) {
        return;
    }
    if (THIS->custom_data[hobOrNobOrDead] == deadMode) {
        if (THIS->custom_data[timer] > 0) {
            THIS->custom_data[timer]--;
        } else {
            SpriteManagerRemoveSprite(THIS);
        }
        return;
    }
    if (THIS->custom_data[hobOrNobOrDead] == waitMode) {
        if (THIS->custom_data[timer] > 0) {
            THIS->custom_data[timer]--;
        } else {
           THIS->custom_data[hobOrNobOrDead] = nobMode;
        }
    }

    // if (THIS->custom_data[timer] == 0 && THIS->custom_data[timerQty] == 0) {
    //     THIS->custom_data[hobOrNobOrDead] = hobMode;
    //     SetSpriteAnim(THIS, hob_walk, 15);
    // }
    if (THIS->custom_data[hobOrNobOrDead] != nobMode && THIS->custom_data[hobOrNobOrDead] != hobMode) {
        return;
    }
    if (isEnemyAligned()) {
        chooseEnemyDirection();
    }
    THIS->custom_data[movement_accumulator] += 4;
    if (THIS->custom_data[movement_accumulator] < 25) {
        return;
    }
    THIS->custom_data[movement_accumulator] -= 5;
    switch (THIS->custom_data[enemy_direction]) {
        case J_LEFT:
            THIS->x--;
        break;
        case J_RIGHT:
            THIS->x++;
        break;
        case J_UP:
            THIS->y--;
        break;
        case J_DOWN:
            THIS->y++;
        break;
    }
}

void DESTROY(void) {
    enemyCountOnScreen--;
    if (spawnTimer == 0) {
        spawnTimer = getEnemySpawnGapTimer();
    }
}
