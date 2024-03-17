#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"
#include "SpriteEnemy.h"

extern uint8_t enemyCount;
extern uint16_t spawnTimer;

const UBYTE nob_walk[] = {4, 0, 1, 2, 1};
const UBYTE hob_walk[] = {4, 4, 5, 6, 5};
const UBYTE nob_dies[] = {1, 3};
const UBYTE hob_dies[] = {1, 7};

void START() {
    SetSpriteAnim(THIS, nob_walk, 15);
    THIS->custom_data[hobOrNob] = nobMode;
    THIS->custom_data[timer] = changeTimer;
    THIS->custom_data[timerQty] = 2;
    THIS->custom_data[enemy_direction] = J_LEFT;
    THIS->custom_data[dead] = FALSE;
    THIS->custom_data[deathTimer] = 45;
    THIS->lim_x = 256;
    THIS->lim_y = 256;
}

void UPDATE() {
    if (THIS->custom_data[6]) {
        if (THIS->custom_data[7] > 0) {
            THIS->custom_data[7]--;
        } else {
            SpriteManagerRemoveSprite(THIS);
        }
        return;
    }
    if (THIS->custom_data[timer] > 0) {
        THIS->custom_data[timer]--;
    }
    if (THIS->custom_data[timer] == 0 && THIS->custom_data[timerQty] > 0) {
        THIS->custom_data[timer] = changeTimer;
        THIS->custom_data[timerQty]--;
    }
    if (THIS->custom_data[timer] == 0 && THIS->custom_data[timerQty] == 0) {
        THIS->custom_data[hobOrNob] = hobMode;
        SetSpriteAnim(THIS, hob_walk, 15);
    }
    switch (THIS->custom_data[enemy_direction]) {
        case J_LEFT:
            if (THIS->x > 8) {
                THIS->x--;
            } else {
                THIS->custom_data[enemy_direction] = J_DOWN;
            }
        break;
        case J_RIGHT:
            if (THIS->x < 232) {
                THIS->x++;
            } else {
                THIS->custom_data[enemy_direction] = J_UP;
            }
        break;
        case J_UP:
            if (THIS->y > 24) {
                THIS->y--;
            } else {
                THIS->custom_data[enemy_direction] = J_LEFT;
            }
        break;
        case J_DOWN:
            if (THIS->y < 168) {
                THIS->y++;
            } else {
                THIS->custom_data[enemy_direction] = J_RIGHT;
            }
        break;
    }
}

void DESTROY() {
    enemyCount--;
    if (spawnTimer == 0) {
        spawnTimer = 500;
    }
}