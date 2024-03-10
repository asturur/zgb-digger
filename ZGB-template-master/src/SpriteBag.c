#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"
#include "SpriteEnemy.h"

#define shakeBeforeFall 60

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
    THIS->custom_data[0] = 1;
    THIS->custom_data[1] = shakeBeforeFall;
    THIS->custom_data[2] = 0;
    THIS->lim_x = 256;
    THIS->lim_y = 256;
    updateBagTiles();
}

void UPDATE() {

}

void DESTROY() {
    
}