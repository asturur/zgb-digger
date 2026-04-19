#include "Banks/SetAutoBank.h"
#include "Keys.h"
#include "SpriteManager.h"
#include "ZGBMain.h"
#include "StateGame.h"
#include "SpriteFireball.h"
#include "SpriteBag.h"
#include "SpritePlayer.h"
#include "Scroll.h"

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

#define moveNoStep 0
#define moveAdvanced 1
#define moveBlockedByBag 2

UBYTE direction;
static UBYTE activeDigDirection;

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

BOOLEAN crushPlayerWithBag(uint16_t bagY) BANKED {
    if (scroll_target == 0 ||
        scroll_target->marked_for_removal ||
        scroll_target->custom_data[death_state] == playerDeathPreGraveWait ||
        isDying) {
        return FALSE;
    }

    if (scroll_target->custom_data[death_state] == playerDeathBagPinned) {
        if ((uint16_t)(bagY + 6) > scroll_target->y) {
            scroll_target->y = (uint8_t)(bagY + 6);
        }
        return TRUE;
    }

    if (scroll_target->custom_data[death_state] != playerDeathNone) {
        return FALSE;
    }

    if ((uint16_t)(bagY + 6) > scroll_target->y) {
        scroll_target->y = (uint8_t)(bagY + 6);
    }
    if (!infiniteLives && lives > 0) {
        lives--;
    }
    updateScore(0);
    scroll_target->custom_data[death_state] = playerDeathBagPinned;
    scroll_target->custom_data[death_timer] = 0;
    scroll_target->custom_data[death_step] = 0;
    scroll_target->custom_data[death_base_y] = (UBYTE)scroll_target->y;
    SetSpriteAnim(scroll_target, anim_dead, 15);
    return TRUE;
}

void finalizePlayerBagCrush(void) BANKED {
    if (scroll_target == 0 ||
        scroll_target->marked_for_removal ||
        scroll_target->custom_data[death_state] != playerDeathBagPinned) {
        return;
    }

    beginDeathFreeze();
    scroll_target->custom_data[death_state] = playerDeathBounce;
    scroll_target->custom_data[death_timer] = 0;
    scroll_target->custom_data[death_step] = 0;
    scroll_target->custom_data[death_base_y] = (UBYTE)scroll_target->y;
    SetSpriteAnim(scroll_target, anim_dead, 15);
}

static void updateDeathSequence(void) {
	switch (THIS->custom_data[death_state]) {
        case playerDeathBagPinned:
            return;
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
    if(_cpu != CGB_TYPE){
        SPRITE_SET_PALETTE(THIS,1);
    }
    setDirection(J_RIGHT);
    activeDigDirection = J_RIGHT;
    setRechargeTime(0);
    THIS->custom_data[death_state] = playerDeathNone;
    THIS->custom_data[player_movement_accumulator] = 0;
    THIS->custom_data[player_last_dig_cell] = 0xFF;
}

static BOOLEAN isColumnDisaligned(void) {
    return MOD_FOR_LARGE_TILE(THIS->x - mapBoundLeft);
}

static BOOLEAN isRowDisaligned(void) {
    return MOD_FOR_LARGE_TILE(THIS->y - mapBoundUp);
}

static UBYTE tryPushStaticBag(void) {
    if (direction != J_LEFT && direction != J_RIGHT) {
        return pushBagNoBag;
    }
    if (isRowDisaligned() || isColumnDisaligned()) {
        return pushBagNoBag;
    }
    return tryPushBagChainFromCell(getMapMetaTileArrayPosition(THIS->x, THIS->y), direction);
}

static BOOLEAN overlapsMetaSprite(UBYTE x1, UBYTE y1, UBYTE x2, UBYTE y2) {
    return x1 < (UBYTE)(x2 + largeTileSize) &&
        (UBYTE)(x1 + largeTileSize) > x2 &&
        y1 < (UBYTE)(y2 + largeTileSize) &&
        (UBYTE)(y1 + largeTileSize) > y2;
}

static Sprite* findHorizontalActiveBagAhead(UBYTE nextX) {
    uint8_t i;
    Sprite* spr;

    SPRITEMANAGER_ITERATE(i, spr) {
        if (!spr->marked_for_removal &&
            spr->type == SpriteBag &&
            spr->custom_data[bagStatus] != stateFalling &&
            overlapsMetaSprite(nextX, THIS->y, spr->x, spr->y)) {
            return spr;
        }
    }
    return 0;
}

static UBYTE tryPushActiveBag(void) {
    Sprite* bag;
    UBYTE nextX;

    if (direction != J_LEFT && direction != J_RIGHT) {
        return pushBagNoBag;
    }
    if (isRowDisaligned() || isColumnDisaligned()) {
        return pushBagNoBag;
    }

    nextX = direction == J_LEFT ? (UBYTE)(THIS->x - 1) : (UBYTE)(THIS->x + 1);
    bag = findHorizontalActiveBagAhead(nextX);
    if (bag == 0) {
        return pushBagNoBag;
    }
    return pushActiveBag(bag, direction, bagPushOwnerPlayer);
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
    return itemMap[getMapMetaTileArrayPosition(THIS->x, edgeY)] == itemBag;
}

static BOOLEAN verticalMoveBlockedByBag(UBYTE nextY) {
    return verticalMoveHitsStaticBag(nextY) || verticalMoveHitsActiveBag(nextY);
}

static UBYTE updatePosition(void) {
    UBYTE nextY;

    THIS->custom_data[player_movement_accumulator] += 4;
    if (THIS->custom_data[player_movement_accumulator] < 5) {
        return moveNoStep;
    }
    THIS->custom_data[player_movement_accumulator] -= 5;
    switch (direction) {
        case J_UP:
            if (THIS->y > mapBoundUp) {
                nextY = THIS->y - 1;
                if (verticalMoveBlockedByBag(nextY)) {
                    THIS->custom_data[player_movement_accumulator] = 0;
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
                    THIS->custom_data[player_movement_accumulator] = 0;
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
    uint16_t recharging = getRechargeTime() > 0;

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

static UBYTE isOppositeDirection(UBYTE dirA, UBYTE dirB) {
    return (dirA == J_LEFT && dirB == J_RIGHT) ||
        (dirA == J_RIGHT && dirB == J_LEFT) ||
        (dirA == J_UP && dirB == J_DOWN) ||
        (dirA == J_DOWN && dirB == J_UP);
}

static UBYTE getLeadingDigCellForDirection(UBYTE moveDirection) {
    uint16_t leadX = THIS->x;
    uint16_t leadY = THIS->y;

    if (moveDirection == J_RIGHT) {
        leadX = (uint16_t)(leadX + largeTileSize - 1);
    } else if (moveDirection == J_DOWN) {
        leadY = (uint16_t)(leadY + largeTileSize - 1);
    }

    return getMapMetaTileArrayPosition(leadX, leadY);
}

static UBYTE getLeadingDigSlotForDirection(UBYTE moveDirection) {
    UBYTE offset;

    if (moveDirection == J_LEFT || moveDirection == J_RIGHT) {
        uint16_t leadX = THIS->x;
        if (moveDirection == J_RIGHT) {
            leadX = (uint16_t)(leadX + largeTileSize - 1);
        }
        offset = MOD_FOR_LARGE_TILE(leadX - mapBoundLeft);
    } else {
        uint16_t leadY = THIS->y;
        if (moveDirection == J_DOWN) {
            leadY = (uint16_t)(leadY + largeTileSize - 1);
        }
        offset = MOD_FOR_LARGE_TILE(leadY - mapBoundUp);
    }

    return offset >> 2;
}

static UBYTE getHorizontalDigStepMask(UBYTE slotIndex) {
    return (UBYTE)(tunnelHorizontalStep1 << slotIndex);
}

static UBYTE getVerticalDigStepMask(UBYTE slotIndex) {
    return (UBYTE)(tunnelVerticalStep1 << slotIndex);
}

static void extendTunnelProgress(UBYTE cell, UBYTE moveDirection, UBYTE slotIndex) {
    if (slotIndex > 3) {
        slotIndex = 3;
    }

    switch (moveDirection) {
        case J_LEFT:
        case J_RIGHT:
            tunnelMap[cell] |= getHorizontalDigStepMask(slotIndex);
            if ((tunnelMap[cell] & tunnelHorizontalCenterMask) == tunnelHorizontalCenterMask) {
                tunnelMap[cell] |= tunnelVerticalCenterMask;
            }
            break;
        case J_UP:
        case J_DOWN:
            tunnelMap[cell] |= getVerticalDigStepMask(slotIndex);
            if ((tunnelMap[cell] & tunnelVerticalCenterMask) == tunnelVerticalCenterMask) {
                tunnelMap[cell] |= tunnelHorizontalCenterMask;
            }
            break;
        default:
            return;
    }
}

static void updateTunnelProgress(void) {
    const UBYTE previousDigCell = THIS->custom_data[player_last_dig_cell];
    UBYTE digDirection = direction;
    UBYTE currentDigCell = getLeadingDigCellForDirection(digDirection);
    UBYTE currentDigSlot = getLeadingDigSlotForDirection(digDirection);

    if (previousDigCell != 0xFF &&
        currentDigCell == previousDigCell &&
        isOppositeDirection(direction, activeDigDirection)) {
        digDirection = activeDigDirection;
        currentDigCell = previousDigCell;
        currentDigSlot = getLeadingDigSlotForDirection(digDirection);
    }

    if (previousDigCell != 0xFF && previousDigCell != currentDigCell) {
        renderMetaCell(previousDigCell);
    }

    extendTunnelProgress(currentDigCell, digDirection, currentDigSlot);
    renderMetaCell(currentDigCell);
    THIS->custom_data[player_last_dig_cell] = currentDigCell;
    activeDigDirection = digDirection;
}

void UPDATE(void) {
    UBYTE moveResult = moveNoStep;

    if (THIS->custom_data[death_state] == playerDeathScoreboardMode) {
        return;
    }

    if (THIS->custom_data[death_state] != playerDeathNone) {
        updateDeathSequence();
        return;
    }
    if(KEY_PRESSED(J_B)) {
        beginTestDeathSequence();
        return;
    }
    if (isDying || paused) {
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
    uint16_t rechargeTime = getRechargeTime();
    if(KEY_PRESSED(J_A)) {
        if (rechargeTime == 0) {
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
    if (rechargeTime > 0) {
        setRechargeTime(rechargeTime - 1);
        if (rechargeTime == 1) {
            updateAnimation();
        }
    }
    if (moving && !isRowDisaligned() && !isColumnDisaligned() && (direction == J_LEFT || direction == J_RIGHT)) {
        UBYTE pushResult = tryPushStaticBag();
        if (pushResult == pushBagNoBag) {
            pushResult = tryPushActiveBag();
        }
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
            updateTunnelProgress();
            runMapSideEffects();
        } else if (moveResult == moveBlockedByBag) {
            moving = FALSE;
        }
    } else {
        THIS->custom_data[player_movement_accumulator] = 0;
    }
    if (!moving) {
        THIS->custom_data[player_movement_accumulator] = 0;
    }
}

void DESTROY(void) {
}
