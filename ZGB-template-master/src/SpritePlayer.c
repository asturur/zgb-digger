#include "Banks/SetAutoBank.h"
#include "Keys.h"
#include "SpriteManager.h"

const UINT8 anim_walk_right[] = {4, 0, 1, 2, 1};
const UINT8 anim_walk_left[] = {4, 9, 10, 11, 10};
const UINT8 anim_walk_up[] = {4, 6, 7, 8, 7};
const UINT8 anim_walk_down[] = {4, 3, 4, 5, 4};
uint8_t direction;

void START() {
    direction = J_RIGHT;
}

UINT8 isColumnDisaligned () {
    return (THIS->x - 8) % 16;
}

UINT8 isRowDisaligned () {
    return (THIS->y - 8) % 16;
}

void updateForRight() {
    if (THIS->x < 232) { // 31 * 8
        THIS->x ++;
        SetSpriteAnim(THIS, anim_walk_right, 15);
    }
}

void updateForLeft() {
    if (THIS->x > 8) { // 1 * 8
        THIS->x --;
        SetSpriteAnim(THIS, anim_walk_left, 15);
    }
}

void updateForUp() {
    if (THIS->y > 24) { // 3 * 8
        THIS->y --;
        SetSpriteAnim(THIS, anim_walk_up, 15);
    }
}

void updateForDown() {
    if (THIS->y < 168) { // 22 * 8
        THIS->y ++;
        SetSpriteAnim(THIS, anim_walk_down, 15);
    }
}

void UPDATE() {
    uint8_t finalDirection = 0;
    if (KEY_PRESSED(J_UP)) {
        if (isColumnDisaligned() && THIS->y > 24) {
            if (direction == J_RIGHT) {
                finalDirection = J_RIGHT;
            } else {
                finalDirection = J_LEFT;
            }
        } else {
            finalDirection = J_UP;
        }
	} 
	if(KEY_PRESSED(J_DOWN)) {
        if (isColumnDisaligned() && THIS->y < 168) {
            if (direction == J_RIGHT) {
                finalDirection = J_RIGHT;
            } else {
                finalDirection = J_LEFT;
            }
        } else {
            finalDirection = J_DOWN;
        }
	}
	if(KEY_PRESSED(J_LEFT)) {
        if (isRowDisaligned() && THIS->x > 8) {
            if (direction == J_UP) {
                finalDirection = J_UP;
            } else {
                finalDirection = J_DOWN;
            }
        } else {
            finalDirection = J_LEFT;
        }
	}
	if(KEY_PRESSED(J_RIGHT)) {
        if (isRowDisaligned() && THIS->x < 232) {
            if (direction == J_UP) {
                finalDirection = J_UP;
            } else {
                finalDirection = J_DOWN;
            }
        } else {
            finalDirection = J_RIGHT;
        }
	}
    direction = finalDirection;
    switch (finalDirection) {
        case J_UP:
            updateForUp();
            break;
        case J_DOWN:
            updateForDown();
            break;
        case J_LEFT:
            updateForLeft();
            break;
        case J_RIGHT:
            updateForRight();
            break;
    }
    
    
}

void DESTROY() {
}