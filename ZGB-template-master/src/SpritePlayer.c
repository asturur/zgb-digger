#include "Banks/SetAutoBank.h"
#include "Keys.h"
#include "SpriteManager.h"

const UINT8 anim_walk_right[] = {4, 0, 1, 2, 1};
const UINT8 anim_walk_left[] = {4, 9, 10, 11, 10};
const UINT8 anim_walk_up[] = {4, 6, 7, 8, 7};
const UINT8 anim_walk_down[] = {4, 3, 4, 5, 4};

void START() {
}

void UPDATE() {
    if(KEY_PRESSED(J_UP)) {
		THIS->y --;
        SetSpriteAnim(THIS, anim_walk_up, 15);
	} 
	if(KEY_PRESSED(J_DOWN)) {
		THIS->y ++;
        SetSpriteAnim(THIS, anim_walk_down, 15);
	}
	if(KEY_PRESSED(J_LEFT)) {
		THIS->x --;
        SetSpriteAnim(THIS, anim_walk_left, 15);
	}
	if(KEY_PRESSED(J_RIGHT)) {
		THIS->x ++;
        SetSpriteAnim(THIS, anim_walk_right, 15);
	}
}

void DESTROY() {
}