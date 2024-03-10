#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"
#include "SpriteEnemy.h"

#define nobMode 0
#define hobMode 1
#define changeTimer 255

const UBYTE nob_walk[] = {4, 0, 1, 2, 1};
const UBYTE hob_walk[] = {4, 4, 5, 6, 5};


// CUSTOM_DATA usage
// 0 Nob/Hob state
// 1 8bit timer
// 2 count of 8 bit timer ( how many 8 bit timer )
// 3 direction
// 4 frigthned state
// 5 has eaten gold

void START() {
    SetSpriteAnim(THIS, nob_walk, 15);
    THIS->custom_data[0] = nobMode;
    THIS->custom_data[1] = changeTimer;
    THIS->custom_data[2] = 2;
    THIS->custom_data[3] = J_LEFT;
    THIS->lim_x = 256;
    THIS->lim_y = 256;
}

void UPDATE() {
    if (THIS->custom_data[1] > 0) {
        THIS->custom_data[1]--;
    }
    if (THIS->custom_data[1] == 0 && THIS->custom_data[2] > 0) {
        THIS->custom_data[1] = changeTimer;
        THIS->custom_data[2]--;
    }
    if (THIS->custom_data[1] == 0 && THIS->custom_data[2] == 0) {
        THIS->custom_data[0] = hobMode;
        SetSpriteAnim(THIS, hob_walk, 15);
    }
    switch (THIS->custom_data[3]) {
        case J_LEFT:
            if (THIS->x > 8) {
                THIS->x--;
            } else {
                THIS->custom_data[3] = J_DOWN;
            }
        break;
        case J_RIGHT:
            if (THIS->x < 232) {
                THIS->x++;
            } else {
                THIS->custom_data[3] = J_UP;
            }
        break;
        case J_UP:
            if (THIS->y > 24) {
                THIS->y--;
            } else {
                THIS->custom_data[3] = J_LEFT;
            }
        break;
        case J_DOWN:
            if (THIS->y < 168) {
                THIS->y++;
            } else {
                THIS->custom_data[3] = J_RIGHT;
            }
        break;
    }
}

void DESTROY() {
}