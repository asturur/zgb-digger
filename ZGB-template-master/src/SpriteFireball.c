#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"
#include "SpriteEnemy.h"
#include "ZGBMain.h"
#include "StateGame.h"
#include "SpriteFireball.h"

extern const UBYTE nob_dies[2];
extern const UBYTE hob_dies[2];

const UBYTE fireball_anim[] = {3, 0, 1, 2};
const UBYTE explosion_anim[] = {3, 3, 4, 5};

void START(void) {
    SetSpriteAnim(THIS, fireball_anim, 15);
    THIS->custom_data[projectile_direction] = 0;
    THIS->custom_data[exploding] = FALSE;
    THIS->custom_data[despawnTimer] = 45;
    THIS->lim_x = 50;
    THIS->lim_y = 50;
}

void UPDATE(void) {
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

    switch ( THIS->custom_data[projectile_direction]) {
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
	                UBYTE enemyMode = spr->custom_data[mode];
	                THIS->custom_data[exploding] = TRUE;
	                spr->custom_data[mode] = deadMode;
	                spr->custom_data[mode_timer] = deathTimer;
					SetSpriteAnim(spr, enemyMode == nobMode ? nob_dies : hob_dies, 15);
	                SetSpriteAnim(THIS, explosion_anim, 15);
	                updateScore(scoreKill);
				}
			}
	}
}

void DESTROY(void) {
    
}
