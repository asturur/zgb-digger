#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"
#include "SpriteEnemy.h"

#define shakeBeforeFall 60
#define stateShaking 2
#define stateStatic 1
#define stateFalling 3

const UBYTE bag_shake[] = {4, 0, 1, 0, 2};
const UBYTE bag_fall[] = {1, 3};
const UBYTE bag_static[] = {1, 0};

// CUSTOM_DATA usage
// 0 state 1 static, 2: shaking: 3: falling
// 1 8bit timer
// 2 fall counter

void updateBagTiles() {
    UBYTE modRight = THIS->x % 8;
    UBYTE modDown = THIS->y % 8;
    UBYTE bagColumn = (THIS->x - modRight) / 8;
    UBYTE bagRow = (THIS->y - modDown) / 8;
    set_bkg_tile_xy(bagColumn, bagRow, 1);
    set_bkg_tile_xy(bagColumn + 1, bagRow, 1);
    set_bkg_tile_xy(bagColumn, bagRow + 1, 1);
    set_bkg_tile_xy(bagColumn + 1, bagRow + 1, 1);
}



void START() {
    SetSpriteAnim(THIS, bag_shake, 15);
    THIS->custom_data[0] = 2;
    THIS->custom_data[1] = shakeBeforeFall;
    THIS->custom_data[2] = 0;
    THIS->lim_x = 256;
    THIS->lim_y = 256;
    updateBagTiles();
}

void UPDATE() {
    // if is shaking continue to shake
    if (THIS->custom_data[0] == stateShaking && THIS->custom_data[1] > 0) {
         THIS->custom_data[1]--;
    // else time to fall down
    } else if (THIS->custom_data[0] == stateShaking && THIS->custom_data[1] == 0) {
        THIS->custom_data[0] = stateFalling;
        SetSpriteAnim(THIS, bag_fall, 15);
    // else if is falling down
    } else if (THIS->custom_data[0] == stateFalling) {
        if ((THIS->y - 8) % 16) {
            THIS->custom_data[2]++;
            THIS->y++;
        }
        if (THIS->y % 16 == 8) {
            uint8_t row = THIS->y / 8;
            uint8_t column = THIS->x / 8;
            // we need to check what the next 4 tiles are doing
            // if at leat one is 0, se the other to 0 and continue falling
            if (get_bkg_tile_xy(column, row) == 0 || get_bkg_tile_xy(column + 1, row) == 0 || get_bkg_tile_xy(column, row + 1) == 0 || get_bkg_tile_xy(column + 1, row + 1) == 0) {
                set_bkg_tile_xy(column, row, 0);
                set_bkg_tile_xy(column + 1, row, 0);
                set_bkg_tile_xy(column, row + 1, 0); 
                set_bkg_tile_xy(column + 1, row + 1, 0);
                THIS->custom_data[2]++;
            THIS->y++;
            }
        }
    }
}

void DESTROY() {
    
}