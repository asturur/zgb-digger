#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"
#include "Sprite.h"
#include "StateGame.h"

const UBYTE static_emerald[] = {1, 0};
const UBYTE static_gold[] = {1, 1};
const UBYTE static_bonus[] = {1, 2};

void START(UBYTE * mode) {
    if(_cpu != CGB_TYPE){
        SPRITE_SET_PALETTE(THIS,1);
    }
    switch (*mode) {
        case 1:
            SetSpriteAnim(THIS, static_emerald, 2);
        break;
        case 2:
            SetSpriteAnim(THIS, static_gold, 2);
        break;
        case 3:
            SetSpriteAnim(THIS, static_bonus, 2);
        break;
    }
    SetAnimationLoop(THIS, FALSE);
    THIS->lim_x = 256;
    THIS->lim_y = 256;
}

void UPDATE(void) {

}

void DESTROY(void) {

}