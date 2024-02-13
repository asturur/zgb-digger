#include "Banks/SetAutoBank.h"
#include "Keys.h"
#include "SpriteManager.h"

extern unsigned char tileMap[768];

const UBYTE anim_walk_right[] = {4, 0, 1, 2, 1};
const UBYTE anim_walk_left[] = {4, 9, 10, 11, 10};
const UBYTE anim_walk_up[] = {4, 6, 7, 8, 7};
const UBYTE anim_walk_down[] = {4, 3, 4, 5, 4};
UBYTE direction;
UBYTE column;
UBYTE row;

void START() {
    direction = J_RIGHT;
}

BOOLEAN isColumnDisaligned () {
    return (THIS->x - 8) % 16;
}

BOOLEAN isRowDisaligned () {
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

void updateMapTiles() {
    // position of digger is the TOP LEFT first pixel of the sprite.
    // this check runs AFTER the digger has moved
    UBYTE modRight = THIS->x % 8;
    UBYTE modDown = THIS->y % 8;
    UBYTE nextColumn = (THIS->x - modRight) / 8;
    UBYTE nextRow = (THIS->y - modDown) / 8;
    UBYTE tile, tileNext, target;
    // TODO optimization: can i run this if only when necessary?
    // if (nextColumn != column || nextRow != row || changeDirection) {
        column = nextColumn;
        row = nextRow;
        switch(direction) {
            case J_UP:
                tile = get_bkg_tile_xy(column, row);
                tileNext = get_bkg_tile_xy(column + 1, row);
                if (tile != 0) {
                    set_bkg_tile_xy(column, row, 0);
                }
                if (tileNext != 0) {
                    set_bkg_tile_xy(column + 1, row, 0);
                }
                break;
            case J_DOWN:
                target = row + (modDown ? 2 : 1);
                tile = get_bkg_tile_xy(column, target);
                tileNext = get_bkg_tile_xy(column + 1, target);
                if (tile != 0) {
                    set_bkg_tile_xy(column, target, 0);
                }
                if (tileNext != 0) {
                    set_bkg_tile_xy(column + 1, target, 0);
                }
                break;
            case J_LEFT:
                // left is a good case, first pixel we cross we can clean up
                tile = get_bkg_tile_xy(column, row);
                tileNext = get_bkg_tile_xy(column, row + 1);
                if (tile != 0) {
                    set_bkg_tile_xy(column, row, 0);
                }
                if (tileNext != 0) {
                    set_bkg_tile_xy(column, row + 1, 0);
                }
                break;
            case J_RIGHT:
                target = column + (modRight ? 2 : 1);
                tile = get_bkg_tile_xy(target, row);
                tileNext = get_bkg_tile_xy(target, row + 1);
                if (tile != 0) {
                    set_bkg_tile_xy(target, row, 0);
                    tileMap[row * 32 + target] = 0;
                }
                if (tileNext != 0) {
                    set_bkg_tile_xy(target, row + 1, 0);
                    tileMap[(row + 1) * 32 + target] = 0;
                }
                break;
        }
    // }
}

void UPDATE() {
    BOOLEAN moving = FALSE;
    BOOLEAN changeDirection = FALSE;
    if (KEY_PRESSED(J_UP)) {
        moving = TRUE;
        if (isColumnDisaligned() && THIS->y > 24) {
            if (direction == J_RIGHT) {
                direction = J_RIGHT;
            } else {
                direction = J_LEFT;
            }
        } else {
            changeDirection = direction != J_UP;
            direction = J_UP;
        }
	} 
	if(KEY_PRESSED(J_DOWN)) {
        moving = TRUE;
        if (isColumnDisaligned() && THIS->y < 168) {
            if (direction == J_RIGHT) {
                direction = J_RIGHT;
            } else {
                direction = J_LEFT;
            }
        } else {
            changeDirection = direction != J_DOWN;
            direction = J_DOWN;
        }
	}
	if(KEY_PRESSED(J_LEFT)) {
        moving = TRUE;
        if (isRowDisaligned() && THIS->x > 8) {
            if (direction == J_UP) {
                direction = J_UP;
            } else {
                direction = J_DOWN;
            }
        } else {
            changeDirection = direction != J_LEFT;
            direction = J_LEFT;
        }
	}
	if(KEY_PRESSED(J_RIGHT)) {
        moving = TRUE;
        if (isRowDisaligned() && THIS->x < 232) {
            if (direction == J_UP) {
                direction = J_UP;
            } else {
                direction = J_DOWN;
            }
        } else {
            changeDirection = direction != J_RIGHT;
            direction = J_RIGHT;
        }
	}
    if (moving) {
        switch (direction) {
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
        updateMapTiles();
    }
    
    
}

void DESTROY() {
}