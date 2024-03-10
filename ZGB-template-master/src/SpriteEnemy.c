#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"
#include "SpriteEnemy.h"

#define nobMode 0
#define hobMode 1
#define changeTimer 255

const UBYTE nob_walk[] = {4, 0, 1, 2, 1};
const UBYTE hob_walk[] = {4, 4, 5, 6, 5};


void START() {
    SetSpriteAnim(THIS, nob_walk, 15);
    THIS->custom_data[0] = nobMode;
    THIS->custom_data[1] = changeTimer;
    THIS->custom_data[2] = 2;
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
}

void DESTROY() {
}