#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"
#include "SpriteEnemy.h"
#include "StateGame.h"

#define crumbleToGoldTime 25

#define undefined 0
#define stateStatic 1
#define stateStartFalling 2
#define stateFalling 3
#define stateCrumbling 4

#define goldStatus 0
#define crumblingTimer 1

#define goldTL 18
#define goldBL 19
#define goldTR 20
#define goldBR 21


const UBYTE gold_crumbling[] = {1, 0};
const UBYTE gold_start_falling[] = {1, 0};
const UBYTE gold_falling[] = {1, 0};
const UBYTE gold_static[] = {1, 0};

// CUSTOM_DATA usage
// 0 state 1 static, 2: shaking: 3: falling
// 1 8bit timer

static void createGoldBackground(void) {
    UBYTE column = TILE_FROM_PIXEL(THIS->x);
    UBYTE row = TILE_FROM_PIXEL(THIS->y);
    updateVideoMemAndMap(column, row, goldTL);
    updateVideoMemAndMap(column + 1, row, goldTR);
    updateVideoMemAndMap(column, row + 1, goldBL);
    updateVideoMemAndMap(column + 1, row + 1, goldBR);
}

void START(void) {
    THIS->custom_data[goldStatus] = undefined;
    THIS->custom_data[crumblingTimer] = crumbleToGoldTime;
    THIS->lim_x = 256;
    THIS->lim_y = 256;
}

void UPDATE(void) {
    uint8_t state = THIS->custom_data[goldStatus];
    // if undefined or static check for below ground
    if (state == undefined) {
        uint8_t column = TILE_FROM_PIXEL(THIS->x);
        uint8_t row = 2 + TILE_FROM_PIXEL(THIS->y);
        if (THIS->y < mapBoundDown -1 && checkTilesFor(column, row, tileBlack)) {
            THIS->custom_data[goldStatus] = stateFalling;
            SetSpriteAnim(THIS, gold_falling, 10);
        } else {
            THIS->custom_data[goldStatus] = stateCrumbling;
            SetSpriteAnim(THIS, gold_crumbling, 10);
        }
        state = THIS->custom_data[goldStatus];
    }
    if (state == stateCrumbling) {
        if (THIS->custom_data[crumblingTimer] == 0) {
            THIS->custom_data[goldStatus] = stateStatic;
            createGoldBackground();
            addOnMap(THIS->x, THIS->y, metaTileGold);
            SpriteManagerRemoveSprite(THIS);
        } else {
            THIS->custom_data[crumblingTimer]--;
        }
    }
}

void DESTROY(void) {
    
}
