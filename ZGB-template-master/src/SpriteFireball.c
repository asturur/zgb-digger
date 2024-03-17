#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"
#include "SpriteEnemy.h"
#include "ZGBMain.h"

// CUSTOM_DATA usage 0-7
#define direction 0
#define exploding 1
#define despawnTimer 2

extern const UBYTE nob_dies[2];
extern const UBYTE hob_dies[2];

const UBYTE fireball_anim[] = {3, 0, 1, 2};
const UBYTE explosion_anim[] = {3, 3, 4, 5};

void START() {
    SetSpriteAnim(THIS, fireball_anim, 15);
    THIS->custom_data[direction] = 0;
    THIS->custom_data[exploding] = FALSE;
    THIS->custom_data[despawnTimer] = 45;
    THIS->lim_x = 50;
    THIS->lim_y = 50;
}

void UPDATE() {
    uint8_t i;
	Sprite* spr;

    if (THIS->custom_data[exploding]) {
        if (THIS->custom_data[despawnTimer] > 0) {
            THIS->custom_data[despawnTimer]--;
        } else {
            SpriteManagerRemoveSprite(THIS);
        }
        return;
    }

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
    SPRITEMANAGER_ITERATE(i, spr) {
		if(spr->type == SpriteEnemy) {
			if(CheckCollision(THIS, spr)) {
                THIS->custom_data[exploding] = TRUE;
                spr->custom_data[6] = TRUE;
				SetSpriteAnim(spr, spr->custom_data[0] == nobMode ? nob_dies : hob_dies, 15);
                SetSpriteAnim(THIS, explosion_anim, 15);
			}
		}
	}
}

void DESTROY() {
    
}