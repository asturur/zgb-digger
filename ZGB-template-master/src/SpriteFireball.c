#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"

const UBYTE fireball_anim[] = {3, 0, 1, 2};
const UBYTE explosion_anim[] = {3, 3, 4, 5};

// CUSTOM_DATA usage
// 0 direction
// 1 exploding

void START() {
    SetSpriteAnim(THIS, fireball_anim, 15);
    THIS->custom_data[0] = 0;
    THIS->custom_data[1] = FALSE;
    THIS->lim_x = 50;
    THIS->lim_y = 50;
}

void UPDATE() {
    switch ( THIS->custom_data[0]) {
        case J_LEFT:
            THIS->x -= 2;
        break;
        case J_RIGHT:
            THIS->x += 2;
        break;
        case J_DOWN:
            THIS->y += 2;
        break;
        case J_UP:
            THIS->y -= 2;
        break;
    }
}

void DESTROY() {
    
}