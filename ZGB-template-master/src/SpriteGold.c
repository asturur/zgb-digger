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

void createGoldBackground(void) {
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
        // start shaking again if the below tiles are digged
        uint8_t column = TILE_FROM_PIXEL(THIS->x);
        // precedence of bitshift is low compared to addition
        uint8_t row = (TILE_FROM_PIXEL(THIS->y)) + 2;
        if (checkTilesFor(column, row, tileBlack) && THIS->y < mapBoundDown -1) {
            THIS->custom_data[goldStatus] = stateFalling;
            SetSpriteAnim(THIS, gold_falling, 10);
        } else {
            THIS->custom_data[goldStatus] = stateCrumbling;
            SetSpriteAnim(THIS, gold_crumbling, 10);
        }
    }
    if (state == stateCrumbling) {
        if (THIS->custom_data[crumblingTimer] == 0) {
            uint8_t column = TILE_FROM_PIXEL(THIS->x);
            // precedence of bitshift is low compared to addition
            uint8_t row = (TILE_FROM_PIXEL(THIS->y)) + 2;
            createGoldBackground();
            addOnMap(THIS->x, THIS->y, metaTileGold);
            SpriteManagerRemoveSprite(THIS);
        } else {
            THIS->custom_data[crumblingTimer]--;
        }
    }
    // else if (THIS->custom_data[bagStatus] == stateCrumbling && THIS->custom_data[crumblingTimer] == 0) {
    //     THIS->custom_data[bagStatus] = stateStaticGold;
    //     SetSpriteAnim(THIS, bag_static_gold, 15);
    // }
    // if (THIS->custom_data[bagStatus] == stateShaking && THIS->custom_data[bagShakingTimer] > 0) {
    //      THIS->custom_data[bagShakingTimer]--;
    // // it starts to fall down
    // } else if (THIS->custom_data[bagStatus] == stateShaking && THIS->custom_data[bagShakingTimer] == 0) {
    //     THIS->custom_data[bagStatus] = stateFalling;
    //     SetSpriteAnim(THIS, bag_fall, 15);
    // // else if is falling down as a bag or as a pile of gold
    // } else if (
    //     (
    //         THIS->custom_data[bagStatus] == stateFalling ||
    //         THIS->custom_data[bagStatus] == stateFallingGold
    //     ) && THIS->y <= mapBoundDown) {
    //         if (MOD_FOR_LARGE_TILE(THIS->y)) {
    //             THIS->custom_data[bagFallCounter]++;
    //             THIS->y++;
    //         } else {
    //             uint8_t column = TILE_FROM_PIXEL(THIS->x);
    //             // precedence of bitshift is low compared to addition
    //             uint8_t row = (TILE_FROM_PIXEL(THIS->y)) + 2;
    //             // we need to check what the next 4 tiles are doing
    //             // if at leat one is 0, se the other to 0 and continue falling
    //             if (checkTilesFor(column, row, tileBlack) && THIS->y < mapBoundDown) {
    //                 setBagTiles(column, row, tileBlack);
    //                 THIS->custom_data[bagFallCounter]++;
    //                 THIS->y++;
    //             } else {
    //                 // solid ground or reach end of map
    //                 if (THIS->custom_data[bagFallCounter] > 40 && THIS->custom_data[bagStatus] == stateFalling) {
    //                     THIS->custom_data[bagStatus] = stateCrumbling;
    //                     THIS->custom_data[crumblingTimer] = crumbleToGoldTime;
    //                     SetSpriteAnim(THIS, bag_gold_crumble, 15);
    //                 } 
    //                 else if( THIS->custom_data[bagStatus] == stateFalling) {
    //                     THIS->custom_data[bagStatus] = stateStatic;
    //                     THIS->custom_data[bagFallCounter] = 0;
    //                     SetSpriteAnim(THIS, bag_static, 15);
    //                 } else if (THIS->custom_data[bagStatus] == stateFallingGold) {
    //                     THIS->custom_data[bagStatus] = stateStaticGold;
    //                     THIS->custom_data[bagFallCounter] = 0;
    //                     SetSpriteAnim(THIS, bag_static_gold, 15);
    //                 }
    //                 THIS->custom_data[bagFallCounter]=0;
    //             }
    //         }
    // } else if (THIS->custom_data[bagStatus] == stateStatic && THIS->y < mapBoundDown - 1) {
    //     // start shaking again if the below tiles are digged
    //     uint8_t column = TILE_FROM_PIXEL(THIS->x);
    //     // precedence of bitshift is low compared to addition
    //     uint8_t row = (TILE_FROM_PIXEL(THIS->y)) + 2;
    //     if (checkTilesFor(column, row, tileBlack)) {
    //         THIS->custom_data[bagStatus] = stateShaking;
    //         THIS->custom_data[bagShakingTimer] = shakeBeforeFall;
    //         THIS->custom_data[bagFallCounter] = 0;
    //         SetSpriteAnim(THIS, bag_shake, 15);
    //     }
    // } else if (THIS->custom_data[bagStatus] == stateStaticGold && THIS->y < mapBoundDown - 1) {
    //     // start shaking again if the below tiles are digged
    //     uint8_t column = TILE_FROM_PIXEL(THIS->x);
    //     // precedence of bitshift is low compared to addition
    //     uint8_t row = (TILE_FROM_PIXEL(THIS->y)) + 2;
    //     if (checkTilesFor(column, row, tileBlack)) {
    //         THIS->custom_data[bagStatus] = stateFallingGold;
    //     }
    // }
}

void DESTROY(void) {
    
}
