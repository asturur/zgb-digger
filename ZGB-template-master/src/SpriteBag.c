#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"
#include "SpriteEnemy.h"
#include "StateGame.h"

#define shakeBeforeFall 60
#define stateStatic 1
#define stateShaking 2
#define stateFalling 3

#define bagStatus 0
#define bagShakingTimer 1
#define bagFallCounter 2

const UBYTE bag_shake[] = {4, 0, 1, 0, 2};
const UBYTE bag_fall[] = {1, 3};
const UBYTE bag_static[] = {1, 0};

// CUSTOM_DATA usage
// 0 state 1 static, 2: shaking: 3: falling
// 1 8bit timer
// 2 fall counter

BOOLEAN checkTilesFor(UBYTE column, UBYTE row, UBYTE type) {
    return get_bkg_tile_xy(column, row) == type ||
        get_bkg_tile_xy(column + 1, row) == type || 
        get_bkg_tile_xy(column, row + 1) == type || 
        get_bkg_tile_xy(column + 1, row + 1) == type;
}

void setBagTiles(UBYTE column, UBYTE row, UBYTE type) {
    set_bkg_tile_xy(column, row, type);
    set_bkg_tile_xy(column + 1, row, type);
    set_bkg_tile_xy(column, row + 1, type);
    set_bkg_tile_xy(column + 1, row + 1, type);
}

void updateBagTiles(void) {
    UBYTE bagColumn = TILE_FROM_PIXEL(THIS->x);
    UBYTE bagRow = TILE_FROM_PIXEL(THIS->y);
    setBagTiles(bagColumn, bagRow, tileGrass);
}

void START(void) {
    SetSpriteAnim(THIS, bag_shake, 15);
    THIS->custom_data[bagStatus] = 2;
    THIS->custom_data[bagShakingTimer] = shakeBeforeFall;
    THIS->custom_data[bagFallCounter] = 0;
    THIS->lim_x = 256;
    THIS->lim_y = 256;
    updateBagTiles();
}

void UPDATE(void) {
    // if is shaking continue to shake
    if (THIS->custom_data[bagStatus] == stateShaking && THIS->custom_data[bagShakingTimer] > 0) {
         THIS->custom_data[1]--;
    // else time to fall down
    } else if (THIS->custom_data[bagStatus] == stateShaking && THIS->custom_data[bagShakingTimer] == 0) {
        THIS->custom_data[0] = stateFalling;
        SetSpriteAnim(THIS, bag_fall, 15);
    // else if is falling down
    } else if (THIS->custom_data[bagStatus] == stateFalling && THIS->y < mapBoundDown - 1) {
        if (MOD_FOR_LARGE_TILE(THIS->y)) {
            THIS->custom_data[bagFallCounter]++;
            THIS->y++;
        } else {
            uint8_t column = TILE_FROM_PIXEL(THIS->x);
            // precedence of bitshift is low compared to addition
            uint8_t row = (TILE_FROM_PIXEL(THIS->y)) + 2;
            // we need to check what the next 4 tiles are doing
            // if at leat one is 0, se the other to 0 and continue falling
            if (checkTilesFor(column, row, tileBlack)) {
                setBagTiles(column, row, 0);
                THIS->custom_data[bagFallCounter]++;
                THIS->y++;
            } else {
                THIS->custom_data[bagStatus] = stateStatic;
                SetSpriteAnim(THIS, bag_static, 15);
            }
        }
    } else if (THIS->custom_data[bagStatus] == stateStatic && THIS->y < mapBoundDown - 1) {
        // start shaking again if the below tiles are digged
        uint8_t column = TILE_FROM_PIXEL(THIS->x);
        // precedence of bitshift is low compared to addition
        uint8_t row = (TILE_FROM_PIXEL(THIS->y)) + 2;
        if (checkTilesFor(column, row, tileBlack)) {
            THIS->custom_data[bagStatus] = stateShaking;
            THIS->custom_data[bagShakingTimer] = shakeBeforeFall;
            THIS->custom_data[bagFallCounter] = 0;
            SetSpriteAnim(THIS, bag_shake, 15);
        }
    } else if (THIS->y = mapBoundDown) {
        THIS->custom_data[bagStatus] = stateStatic;
        SetSpriteAnim(THIS, bag_static, 15);
    }
}

void DESTROY(void) {
    
}