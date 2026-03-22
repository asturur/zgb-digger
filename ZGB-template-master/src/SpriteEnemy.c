#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"
#include "SpriteEnemy.h"
#include "StateGame.h"

extern uint8_t enemyCountOnScreen;
extern uint8_t spawnTimer;
extern uint8_t isDying;

const UBYTE nob_walk[] = {4, 0, 1, 2, 1};
const UBYTE hob_walk[] = {4, 4, 5, 6, 5};
const UBYTE nob_dies[] = {1, 3};
const UBYTE hob_dies[] = {1, 7};



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
    THIS->custom_data[movement_accumulator] += 4;
    if (THIS->custom_data[movement_accumulator] < 25) {
        return;
    }
    THIS->custom_data[movement_accumulator] -= 5;
    switch (THIS->custom_data[enemy_direction]) {
        case J_LEFT:
            if (THIS->x > mapBoundLeft) {
                THIS->x--;
            } else {
                THIS->custom_data[enemy_direction] = J_DOWN;
                THIS->custom_data[movement_accumulator] = 0;
            }
        break;
        case J_RIGHT:
            if (THIS->x < mapBoundRight) {
                THIS->x++;
            } else {
                THIS->custom_data[enemy_direction] = J_UP;
                THIS->custom_data[movement_accumulator] = 0;
            }
        break;
        case J_UP:
            if (THIS->y > mapBoundUp) {
                THIS->y--;
            } else {
                THIS->custom_data[enemy_direction] = J_LEFT;
                THIS->custom_data[movement_accumulator] = 0;
            }
        break;
        case J_DOWN:
            if (THIS->y < mapBoundDown) {
                THIS->y++;
            } else {
                THIS->custom_data[enemy_direction] = J_RIGHT;
                THIS->custom_data[movement_accumulator] = 0;
            }
        break;
    }
}

void DESTROY(void) {
    enemyCountOnScreen--;
    if (spawnTimer == 0) {
        spawnTimer = getEnemySpawnGapTimer();
    }
}
