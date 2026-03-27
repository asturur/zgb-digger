#include "Banks/SetAutoBank.h"
#include "Keys.h"
#include "SpriteManager.h"
#include "ZGBMain.h"
#include "StateGame.h"
#include "SpriteFireball.h"
#include "SpritePlayer.h"

extern void runMapSideEffects(void);
extern void killPlayer(void);
extern uint8_t isDying;

const UBYTE anim_walk_right[] = {4, 0, 1, 2, 1};
const UBYTE anim_walk_down[] = {4, 3, 4, 5, 4};
const UBYTE anim_walk_up[] = {4, 6, 7, 8, 7};
const UBYTE anim_walk_left[] = {4, 9, 10, 11, 10};

const UBYTE discharged_right[] = {4, 12, 13, 14, 13};
const UBYTE discharged_down[] = {4, 15, 16, 17, 16};
const UBYTE discharged_up[] = {4, 18, 19, 20, 19};
const UBYTE discharged_left[] = {4, 21, 22, 23, 22};

const UBYTE anim_dead[] = {1, 24};

UBYTE direction;
UBYTE oppositeDirection;
UBYTE column;
UBYTE row;

void setDirection(UBYTE dir) {
  direction = dir;
  switch (dir) {
    case J_DOWN:
        oppositeDirection = J_UP;
        break;
    case J_UP:
        oppositeDirection = J_DOWN;
        break;
    case J_LEFT:
        oppositeDirection = J_RIGHT;
        break;
    case J_RIGHT:
        oppositeDirection = J_LEFT;
        break;
  }
}

void START(void) {
    setDirection(J_RIGHT);
    THIS->custom_data[custom_data_recharge] = 0;
    THIS->custom_data[death_animation] = 0;
    THIS->custom_data[movement_accumulator] = 0;
}

BOOLEAN isColumnDisaligned(void) {
    return MOD_FOR_LARGE_TILE(THIS->x - mapBoundLeft);
}

BOOLEAN isRowDisaligned(void) {
    return MOD_FOR_LARGE_TILE(THIS->y - mapBoundUp);
}

void updatePosition(void) {
    THIS->custom_data[movement_accumulator] += 4;
    if (THIS->custom_data[movement_accumulator] < 5) {
        return;
    }
    THIS->custom_data[movement_accumulator] -= 5;
    switch (direction) {
        case J_UP:
            if (THIS->y > mapBoundUp) {
                THIS->y --;
            }
            break;
        case J_DOWN:
            if (THIS->y < mapBoundDown) {
                THIS->y ++;
            }
            break;
        case J_LEFT:
            if (THIS->x > mapBoundLeft) {
                THIS->x--;
            }
            break;
        case J_RIGHT:
            if (THIS->x < mapBoundRight) {
                THIS->x ++;
            }
            break;
    }
}

void updateAnimation(void) {
    switch (direction) {
        case J_UP:
            SetSpriteAnim(THIS, THIS->custom_data[custom_data_recharge] > 0 ? discharged_up : anim_walk_up, 15);
            break;
        case J_DOWN:
            SetSpriteAnim(THIS, THIS->custom_data[custom_data_recharge] > 0 ? discharged_down : anim_walk_down, 15);
            break;
        case J_LEFT:
            SetSpriteAnim(THIS, THIS->custom_data[custom_data_recharge] > 0 ? discharged_left : anim_walk_left, 15);
            break;
        case J_RIGHT:
            SetSpriteAnim(THIS, THIS->custom_data[custom_data_recharge] > 0 ? discharged_right : anim_walk_right, 15);
            break;
    }
}

void updateMapTiles(void) {
    // position of digger is the TOP LEFT first pixel of the sprite.
    // this check runs AFTER the digger has moved
    uint8_t nextColumn = TILE_FROM_PIXEL(THIS->x);
    uint8_t nextRow = TILE_FROM_PIXEL(THIS->y);
    UBYTE tile, tileNext, target;
    // TODO optimization: can i run this if only when necessary?
    // if (nextColumn != column || nextRow != row || changeDirection) {
        column = nextColumn;
        row = nextRow;
        switch(direction) {
            case J_UP:
                tile = getTileMapTile(column, row);
                tileNext = getTileMapTile(column + 1, row);
                if (tile != tileBlack) {
                    updateVideoMemAndMap(column, row, tileBlack);
                }
                if (tileNext != tileBlack) {
                    updateVideoMemAndMap(column + 1, row, tileBlack);
                }
                break;
            case J_DOWN:
                target = row + (MOD_FOR_TILE(THIS->y) ? 2 : 1);
                tile = getTileMapTile(column, target);
                tileNext = getTileMapTile(column + 1, target);
                if (tile != tileBlack) {
                    updateVideoMemAndMap(column, target, tileBlack);
                }
                if (tileNext != tileBlack) {
                    updateVideoMemAndMap(column + 1, target, tileBlack);
                }
                break;
            case J_LEFT:
                // left is a good case, first pixel we cross we can clean up
                tile = getTileMapTile(column, row);
                tileNext = getTileMapTile(column, row + 1);
                if (tile != tileBlack) {
                    updateVideoMemAndMap(column, row, tileBlack);
                }
                if (tileNext != tileBlack) {
                    updateVideoMemAndMap(column, row + 1, tileBlack);
                }
                break;
            case J_RIGHT:
                target = column + (MOD_FOR_TILE(THIS->x) ? 2 : 1);
                tile = getTileMapTile(target, row);
                tileNext = getTileMapTile(target, row + 1);
                if (tile != tileBlack) {
                    updateVideoMemAndMap(target, row, tileBlack);
                }
                if (tileNext != tileBlack) {
                    updateVideoMemAndMap(target, row + 1, tileBlack);
                }
                break;
        }
    // }
}

void UPDATE(void) {
    uint8_t d = THIS->custom_data[death_animation];
    if (d > 0) {
        if (d == 26) {
            THIS->y -= 8;
        } else if (d == 25) {
            THIS->y -= 4;
        } else if (d & 0x01) {
            THIS->y += 1;
        }
        THIS->custom_data[death_animation]--;
        SetSpriteAnim(THIS, anim_dead, 15);
        if (d == 1) {
            SpriteManagerRemoveSprite(THIS);
        }
        return;
    }
    // DEBUG FOR DEATH ANIMATION
    if(KEY_PRESSED(J_B)) {
        killPlayer();
        THIS->custom_data[death_animation] = 8;
    }
    if (isDying == 1) {
		return;
	}
    BOOLEAN moving = FALSE;
    BOOLEAN changeDirection = FALSE;
    if (KEY_PRESSED(J_UP)) {
        moving = TRUE;
        if (isColumnDisaligned() && THIS->y > mapBoundUp) {
            if (direction == J_RIGHT) {
                setDirection(J_RIGHT);
            } else {
                setDirection(J_LEFT);
            }
        } else {
            changeDirection = direction != J_UP;
           setDirection(J_UP);
        }
	} 
	if(KEY_PRESSED(J_DOWN)) {
        moving = TRUE;
        if (isColumnDisaligned() && THIS->y < mapBoundDown) {
            if (direction == J_RIGHT) {
               setDirection(J_RIGHT);
            } else {
               setDirection(J_LEFT);
            }
        } else {
            changeDirection = direction != J_DOWN;
           setDirection(J_DOWN);
        }
	}
	if(KEY_PRESSED(J_LEFT)) {
        moving = TRUE;
        if (isRowDisaligned() && THIS->x > mapBoundLeft) {
            if (direction == J_UP) {
               setDirection(J_UP);
            } else {
               setDirection(J_DOWN);
            }
        } else {
            changeDirection = direction != J_LEFT;
           setDirection(J_LEFT);
        }
	}
	if(KEY_PRESSED(J_RIGHT)) {
        moving = TRUE;
        if (isRowDisaligned() && THIS->x < mapBoundRight) {
            if (direction == J_UP) {
               setDirection(J_UP);
            } else {
               setDirection(J_DOWN);
            }
        } else {
            changeDirection = direction != J_RIGHT;
           setDirection(J_RIGHT);
        }
	}
    if(KEY_PRESSED(J_A)) {
        if (THIS->custom_data[custom_data_recharge] == 0) {
            uint8_t spriteX = 0;
            uint8_t spriteY = 0;
            THIS->custom_data[custom_data_recharge] = 255;
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
            fireball->custom_data[projectile_direction] = direction;
            if (!moving) {
                updateAnimation();
            }
        }
	}
    if (THIS->custom_data[custom_data_recharge] > 0) {
        THIS->custom_data[custom_data_recharge]--;
        if (THIS->custom_data[custom_data_recharge] == 0) {
            updateAnimation();
        }
    }
    if (moving) {
        updatePosition();
        updateAnimation();
        updateMapTiles();
        runMapSideEffects();
    } else {
        THIS->custom_data[movement_accumulator] = 0;
    }
}

void DESTROY(void) {
}
