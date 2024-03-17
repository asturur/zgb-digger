#include "Banks/SetAutoBank.h"
#include "Keys.h"
#include "SpriteManager.h"
#include "ZGBMain.h"
#include "StateGame.h"

extern unsigned char tileMap[768];
extern void runMapSideEffects();

const UBYTE anim_walk_right[] = {4, 0, 1, 2, 1};
const UBYTE anim_walk_left[] = {4, 9, 10, 11, 10};
const UBYTE anim_walk_up[] = {4, 6, 7, 8, 7};
const UBYTE anim_walk_down[] = {4, 3, 4, 5, 4};
UBYTE direction;
UBYTE column;
UBYTE row;

// CUSTOM_DATA usage
// 0 rechargetime

void START() {
    direction = J_RIGHT;
    THIS->custom_data[0] = 0;
}

BOOLEAN isColumnDisaligned () {
    return (THIS->x - mapBoundLeft) % 16;
}

BOOLEAN isRowDisaligned () {
    return (THIS->y - mapBoundLeft) % 16;
}

void updateForRight() {
    if (THIS->x < mapBoundRight) { // 31 * 8
        THIS->x ++;
        SetSpriteAnim(THIS, anim_walk_right, 15);
    }
}

void updateForLeft() {
    if (THIS->x > mapBoundLeft) { // 1 * 8
        THIS->x--;
        SetSpriteAnim(THIS, anim_walk_left, 15);
    }
}

void updateForUp() {
    if (THIS->y > mapBoundUp) { // 3 * 8
        THIS->y --;
        SetSpriteAnim(THIS, anim_walk_up, 15);
    }
}

void updateForDown() {
    if (THIS->y < mapBoundDown) { // 22 * 8
        THIS->y ++;
        SetSpriteAnim(THIS, anim_walk_down, 15);
    }
}

void updateMapTiles() {
    // position of digger is the TOP LEFT first pixel of the sprite.
    // this check runs AFTER the digger has moved
    UBYTE modRight = THIS->x % tileSize;
    UBYTE modDown = THIS->y % tileSize;
    UBYTE nextColumn = (THIS->x - modRight) << tileSizeBitShift;
    UBYTE nextRow = (THIS->y - modDown) << tileSizeBitShift;
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
                    tileMap[row * 32 + column] = 0;
                }
                if (tileNext != 0) {
                    set_bkg_tile_xy(column + 1, row, 0);
                    tileMap[row * 32 + column + 1] = 0;
                }
                break;
            case J_DOWN:
                target = row + (modDown ? 2 : 1);
                tile = get_bkg_tile_xy(column, target);
                tileNext = get_bkg_tile_xy(column + 1, target);
                if (tile != 0) {
                    set_bkg_tile_xy(column, target, 0);
                    tileMap[target * 32 + column] = 0;
                }
                if (tileNext != 0) {
                    set_bkg_tile_xy(column + 1, target, 0);
                    tileMap[target * 32 + column + 1] = 0;
                }
                break;
            case J_LEFT:
                // left is a good case, first pixel we cross we can clean up
                tile = get_bkg_tile_xy(column, row);
                tileNext = get_bkg_tile_xy(column, row + 1);
                if (tile != 0) {
                    set_bkg_tile_xy(column, row, 0);
                    tileMap[row * 32 + column] = 0;
                }
                if (tileNext != 0) {
                    set_bkg_tile_xy(column, row + 1, 0);
                    tileMap[(row + 1) * 32 + column] = 0;
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
        if (isColumnDisaligned() && THIS->y > mapBoundUp) {
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
        if (isColumnDisaligned() && THIS->y < mapBoundDown) {
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
        if (isRowDisaligned() && THIS->x > mapBoundLeft) {
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
        if (isRowDisaligned() && THIS->x < mapBoundRight) {
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
    if(KEY_PRESSED(J_A)) {
        if (THIS->custom_data[0] == 0) {
            uint8_t spriteX = 0;
            uint8_t spriteY = 0;
            THIS->custom_data[0] = 255;
            switch (direction) {
                case J_UP:
                    spriteX = THIS->x + 4;
                    spriteY =  THIS->y - 4;
                    break;
                case J_DOWN:
                    spriteX = THIS->x + 4;
                    spriteY =  THIS->y + 12;
                    break;
                case J_LEFT:
                    spriteX = THIS->x - 4;
                    spriteY =  THIS->y + 6;
                    break;
                case J_RIGHT:
                    spriteX = THIS->x + 12;
                    spriteY =  THIS->y + 6;
                    break;
                default:
                    break;
            }
            Sprite *fireball = SpriteManagerAdd(SpriteFireball, spriteX, spriteY);
            fireball->custom_data[0] = direction;
        }
	}
    if (THIS->custom_data[0] > 0) {
        THIS->custom_data[0]--;
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
        runMapSideEffects();
    }
    
    
}

void DESTROY() {
}