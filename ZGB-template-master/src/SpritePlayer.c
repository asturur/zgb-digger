#include "Banks/SetAutoBank.h"
#include "Keys.h"
#include "SpriteManager.h"
#include "ZGBMain.h"
#include "StateGame.h"
#include "SpriteFireball.h"
#include "SpriteBag.h"
#include "SpritePlayer.h"

extern unsigned char levelMap[150];
extern void runMapSideEffects(void);
extern UBYTE getMapMetaTileArrayPosition(uint16_t x, uint16_t y);
const UBYTE anim_walk_right[] = {4, 0, 1, 2, 1};
const UBYTE anim_walk_down[] = {4, 3, 4, 5, 4};
const UBYTE anim_walk_up[] = {4, 6, 7, 8, 7};
const UBYTE anim_walk_left[] = {4, 9, 10, 11, 10};

const UBYTE discharged_right[] = {4, 12, 13, 14, 13};
const UBYTE discharged_down[] = {4, 15, 16, 17, 16};
const UBYTE discharged_up[] = {4, 18, 19, 20, 19};
const UBYTE discharged_left[] = {4, 21, 22, 23, 22};

const UBYTE anim_dead[] = {1, 24};

#define deathBounceStepFrames originalTickToGameBoyFrameRatio
#define deathPreGraveWaitFrames (5u * originalTickToGameBoyFrameRatio)
#define deathFinalHoldFrames (60u * originalTickToGameBoyFrameRatio)
#define graveEmergeFrames 128u
#define deathRespawnFrames (graveEmergeFrames + deathFinalHoldFrames)
#define graveSpawnYOffset 1

static const UBYTE deathBounceOffsets[] = {3, 5, 6, 6, 5, 3, 0};

#define pushBagNoBag 0
#define pushBagStarted 1
#define pushBagBlocked 2
#define moveNoStep 0
#define moveAdvanced 1
#define moveBlockedByBag 2

UBYTE direction;
UBYTE oppositeDirection;
UBYTE column;
UBYTE row;

static uint16_t getRechargeTime(void) {
    return ((uint16_t)THIS->custom_data[recharge_time_hi] << 8) | THIS->custom_data[recharge_time_lo];
}

static void setRechargeTime(uint16_t value) {
    THIS->custom_data[recharge_time_lo] = (UBYTE)value;
    THIS->custom_data[recharge_time_hi] = (UBYTE)(value >> 8);
}

static uint16_t getFireRechargeFrames(void) {
    UBYTE level = currentLevel;

    if (level == 0) {
        level = 1;
    }

    return ((uint16_t)60 + ((uint16_t)level * 3u)) * originalTickToGameBoyFrameRatio;
}

static void setDirection(UBYTE dir) {
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

static void beginTestDeathSequence(void) {
	if (THIS->custom_data[death_state] != playerDeathNone || isDying) {
		return;
	}

	if (!infiniteLives && lives > 0) {
		lives--;
	}
	updateScore(0);
	beginDeathFreeze();
	THIS->custom_data[death_state] = playerDeathBounce;
	THIS->custom_data[death_timer] = 0;
	THIS->custom_data[death_step] = 0;
	THIS->custom_data[death_base_y] = (UBYTE)THIS->y;
	SetSpriteAnim(THIS, anim_dead, 15);
}

static void updateDeathSequence(void) {
	switch (THIS->custom_data[death_state]) {
		case playerDeathBounce:
			if (THIS->custom_data[death_timer] > 0) {
				THIS->custom_data[death_timer]--;
				return;
			}

			if (THIS->custom_data[death_step] < (UBYTE)sizeof(deathBounceOffsets)) {
				THIS->y = THIS->custom_data[death_base_y] - deathBounceOffsets[THIS->custom_data[death_step]];
				THIS->custom_data[death_step]++;
				THIS->custom_data[death_timer] = deathBounceStepFrames;
			}

			if (THIS->custom_data[death_step] >= (UBYTE)sizeof(deathBounceOffsets)) {
				THIS->y = THIS->custom_data[death_base_y];
				THIS->custom_data[death_state] = playerDeathPreGraveWait;
				THIS->custom_data[death_timer] = deathPreGraveWaitFrames;
			}
			break;
		case playerDeathPreGraveWait:
			if (THIS->custom_data[death_timer] > 0) {
				THIS->custom_data[death_timer]--;
				return;
			}

			SetVisible(THIS, FALSE);
			SpriteManagerAdd(SpriteGrave, THIS->x, THIS->y - graveSpawnYOffset);
			playDeathMusic();
			queueDeathRespawn(deathRespawnFrames);
			SpriteManagerRemoveSprite(THIS);
			break;
		default:
			break;
	}
}

void START(void) {
    setDirection(J_RIGHT);
    setRechargeTime(0);
    THIS->custom_data[death_state] = playerDeathNone;
    THIS->custom_data[movement_accumulator] = 0;
}

static BOOLEAN isColumnDisaligned(void) {
    return MOD_FOR_LARGE_TILE(THIS->x - mapBoundLeft);
}

static BOOLEAN isRowDisaligned(void) {
    return MOD_FOR_LARGE_TILE(THIS->y - mapBoundUp);
}

static UBYTE tryPushStaticBag(void) {
    UBYTE bagCells[mapMetaWidth];
    Sprite* activatedBags[mapMetaWidth];
    UBYTE chainCount = 0;
    UBYTE currentCell;
    UBYTE currentColumn;
    UBYTE bagCell;
    UBYTE destinationCell;
    UBYTE destinationValue;
    UBYTE scanColumn;
    UBYTE idx;

    if (direction != J_LEFT && direction != J_RIGHT) {
        return pushBagNoBag;
    }
    if (isRowDisaligned() || isColumnDisaligned()) {
        return pushBagNoBag;
    }

    currentCell = getMapMetaTileArrayPosition(THIS->x, THIS->y);
    currentColumn = currentCell % mapMetaWidth;

    if (direction == J_LEFT) {
        if (currentColumn == 0) {
            return pushBagNoBag;
        }
        bagCell = currentCell - 1;
    } else {
        if (currentColumn == mapMetaWidth - 1) {
            return pushBagNoBag;
        }
        bagCell = currentCell + 1;
    }

    if ((levelMap[bagCell] & metaTileBag) == 0) {
        return pushBagNoBag;
    }

    destinationCell = bagCell;
    scanColumn = bagCell % mapMetaWidth;
    while ((levelMap[destinationCell] & metaTileBag) != 0) {
        if (chainCount == mapMetaWidth) {
            return pushBagBlocked;
        }
        bagCells[chainCount++] = destinationCell;
        if (direction == J_LEFT) {
            if (scanColumn == 0) {
                return pushBagBlocked;
            }
            destinationCell--;
            scanColumn--;
        } else {
            if (scanColumn == mapMetaWidth - 1) {
                return pushBagBlocked;
            }
            destinationCell++;
            scanColumn++;
        }
    }

    destinationValue = levelMap[destinationCell];
    if ((destinationValue & (metaTileBag | metaTileEmerald | metaTileGold)) != 0) {
        return pushBagBlocked;
    }

    for (idx = 0; idx != chainCount; ++idx) {
        activatedBags[idx] = 0;
    }

    for (idx = chainCount; idx != 0; --idx) {
        Sprite* bagSprite = activateBag(bagCells[idx - 1]);
        if (bagSprite == 0) {
            while (idx < chainCount) {
                restoreStaticBag(activatedBags[idx]);
                idx++;
            }
            return pushBagBlocked;
        }
        bagSprite->custom_data[bagDirection] = direction;
        setBagState(bagSprite, statePushing);
        activatedBags[idx - 1] = bagSprite;
    }

    return pushBagStarted;
}

static BOOLEAN overlapsMetaSprite(UBYTE x1, UBYTE y1, UBYTE x2, UBYTE y2) {
    return x1 < (UBYTE)(x2 + largeTileSize) &&
        (UBYTE)(x1 + largeTileSize) > x2 &&
        y1 < (UBYTE)(y2 + largeTileSize) &&
        (UBYTE)(y1 + largeTileSize) > y2;
}

static BOOLEAN verticalMoveHitsActiveBag(UBYTE nextY) {
    uint8_t i;
    Sprite* spr;

    SPRITEMANAGER_ITERATE(i, spr) {
        if (spr->type == SpriteBag &&
            spr->custom_data[bagStatus] != stateFalling &&
            overlapsMetaSprite(THIS->x, nextY, spr->x, spr->y)) {
            return TRUE;
        }
    }
    return FALSE;
}

static BOOLEAN verticalMoveHitsStaticBag(UBYTE nextY) {
    UBYTE edgeY;

    if (THIS->x < mapBoundLeft || THIS->x > mapBoundRight || nextY < mapBoundUp || nextY > mapBoundDown) {
        return FALSE;
    }
    edgeY = direction == J_UP ? nextY : (UBYTE)(nextY + largeTileSize - 1);
    return (levelMap[getMapMetaTileArrayPosition(THIS->x, edgeY)] & metaTileBag) != 0;
}

static BOOLEAN verticalMoveBlockedByBag(UBYTE nextY) {
    return verticalMoveHitsStaticBag(nextY) || verticalMoveHitsActiveBag(nextY);
}

static UBYTE updatePosition(void) {
    UBYTE nextY;

    THIS->custom_data[movement_accumulator] += 4;
    if (THIS->custom_data[movement_accumulator] < 5) {
        return moveNoStep;
    }
    THIS->custom_data[movement_accumulator] -= 5;
    switch (direction) {
        case J_UP:
            if (THIS->y > mapBoundUp) {
                nextY = THIS->y - 1;
                if (verticalMoveBlockedByBag(nextY)) {
                    THIS->custom_data[movement_accumulator] = 0;
                    return moveBlockedByBag;
                }
                THIS->y = nextY;
                return moveAdvanced;
            }
            break;
        case J_DOWN:
            if (THIS->y < mapBoundDown) {
                nextY = THIS->y + 1;
                if (verticalMoveBlockedByBag(nextY)) {
                    THIS->custom_data[movement_accumulator] = 0;
                    return moveBlockedByBag;
                }
                THIS->y = nextY;
                return moveAdvanced;
            }
            break;
        case J_LEFT:
            if (THIS->x > mapBoundLeft) {
                THIS->x--;
                return moveAdvanced;
            }
            break;
        case J_RIGHT:
            if (THIS->x < mapBoundRight) {
                THIS->x ++;
                return moveAdvanced;
            }
            break;
    }
    return moveNoStep;
}

static void updateAnimation(void) {
    BOOLEAN recharging = getRechargeTime() > 0;

    switch (direction) {
        case J_UP:
            SetSpriteAnim(THIS, recharging ? discharged_up : anim_walk_up, 15);
            break;
        case J_DOWN:
            SetSpriteAnim(THIS, recharging ? discharged_down : anim_walk_down, 15);
            break;
        case J_LEFT:
            SetSpriteAnim(THIS, recharging ? discharged_left : anim_walk_left, 15);
            break;
        case J_RIGHT:
            SetSpriteAnim(THIS, recharging ? discharged_right : anim_walk_right, 15);
            break;
    }
}

static void updateMapTiles(void) {
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
    UBYTE moveResult = moveNoStep;

    if (THIS->custom_data[death_state] != playerDeathNone) {
        updateDeathSequence();
        return;
    }
    if(KEY_PRESSED(J_B)) {
        beginTestDeathSequence();
        return;
    }
    if (isDying) {
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
        if (getRechargeTime() == 0) {
            uint8_t spriteX = 0;
            uint8_t spriteY = 0;
            setRechargeTime(getFireRechargeFrames());
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
            if (fireball != 0) {
                fireball->custom_data[projectile_direction] = direction;
                if (!moving) {
                    updateAnimation();
                }
            }
        }
	}
    if (getRechargeTime() > 0) {
        uint16_t rechargeTime = getRechargeTime() - 1;
        setRechargeTime(rechargeTime);
        if (rechargeTime == 0) {
            updateAnimation();
        }
    }
    if (moving && !isRowDisaligned() && !isColumnDisaligned() && (direction == J_LEFT || direction == J_RIGHT)) {
        UBYTE pushResult = tryPushStaticBag();
        if (pushResult == pushBagBlocked) {
            moving = FALSE;
            if (changeDirection) {
                updateAnimation();
            }
        }
    }
    if (moving) {
        updateAnimation();
        moveResult = updatePosition();
        if (moveResult == moveAdvanced) {
            updateMapTiles();
            runMapSideEffects();
        } else if (moveResult == moveBlockedByBag) {
            moving = FALSE;
        }
    } else {
        THIS->custom_data[movement_accumulator] = 0;
    }
    if (!moving) {
        THIS->custom_data[movement_accumulator] = 0;
    }
}

void DESTROY(void) {
}
