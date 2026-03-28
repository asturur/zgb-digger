#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"
#include "SpriteEnemy.h"
#include "SpriteBag.h"
#include "ZGBMain.h"
#include "StateGame.h"

extern unsigned char levelMap[150];
extern uint8_t enemyCountOnScreen;
extern uint8_t spawnTimer;
extern uint8_t isDying;
extern UBYTE getMapMetaTileArrayPosition(uint16_t x, uint16_t y);

const UBYTE nob_walk[] = {4, 0, 1, 2, 1};
const UBYTE hob_walk[] = {4, 4, 5, 6, 5};
const UBYTE nob_dies[] = {1, 3};
const UBYTE hob_dies[] = {1, 7};

static BOOLEAN enemyUsesHobAnimation(Sprite* enemy) {
    return enemy->custom_data[mode] == hobMode ||
        enemy->anim_data == hob_walk ||
        enemy->anim_data == hob_dies;
}

static void setEnemyModeFor(Sprite* enemy, UBYTE enemyMode) {
    BOOLEAN useHobDeathAnim = enemyUsesHobAnimation(enemy);
    enemy->custom_data[mode] = enemyMode;

    switch (enemyMode) {
        case nobMode:
            enemy->custom_data[mode_timer] = 0;
            SetSpriteAnim(enemy, nob_walk, 15);
        break;
        case hobMode:
            enemy->custom_data[mode_timer] = 0;
            SetSpriteAnim(enemy, hob_walk, 15);
        break;
        case deadMode:
            enemy->custom_data[mode_timer] = deathTimer;
            SetSpriteAnim(enemy, useHobDeathAnim ? hob_dies : nob_dies, 15);
        break;
        case waitMode:
            enemy->custom_data[mode_timer] = initialWaitTime;
        break;
        case crushedMode:
            enemy->custom_data[mode_timer] = 0;
            SetSpriteAnim(enemy, useHobDeathAnim ? hob_dies : nob_dies, 15);
        break;
    }
}

static void setEnemyMode(UBYTE enemyMode) {
    setEnemyModeFor(THIS, enemyMode);
}

void crushEnemy(Sprite* enemy) BANKED {
    UBYTE enemyMode = enemy->custom_data[mode];

    if (enemyMode == deadMode || enemyMode == waitMode || enemyMode == crushedMode) {
        return;
    }

    setEnemyModeFor(enemy, crushedMode);
}

static UBYTE oppositeDirectionBit(UBYTE direction) {
    switch (direction) {
        case J_LEFT:
            return J_RIGHT;
        case J_RIGHT:
            return J_LEFT;
        case J_DOWN:
            return J_UP;
        case J_UP:
            return J_DOWN;
        default:
            return 0;
    }
}

static BOOLEAN isEnemyAligned(void) {
    return MOD_FOR_LARGE_TILE(THIS->x - mapBoundLeft) == 0 && MOD_FOR_LARGE_TILE(THIS->y - mapBoundUp) == 0;
}

static BOOLEAN followCrushingBag(void) {
    uint8_t i;
    Sprite* spr;

    SPRITEMANAGER_ITERATE(i, spr) {
        if (spr->type == SpriteBag && spr->custom_data[bagStatus] == stateFalling) {
            if (CheckCollision(THIS, spr) && THIS->y >= spr->y) {
                THIS->y = spr->y;
                return TRUE;
            }
        }
    }
    return FALSE;
}

static UBYTE getEnemyCurrentTunnel(void) {

    UBYTE currentCell;

    if (THIS->x < mapBoundLeft || THIS->y < mapBoundUp || THIS->x > mapBoundRight || THIS->y > mapBoundDown) {
        return 0;
    }

    currentCell = getMapMetaTileArrayPosition(THIS->x, THIS->y);
	return levelMap[currentCell];
}

static BOOLEAN enemyCanMove(UBYTE direction, UBYTE tunnel) {
    if ((tunnel & oppositeDirectionBit(direction)) == 0) {
        return FALSE;
    }

    switch (direction) {
        case J_LEFT:
            return THIS->x > mapBoundLeft;
        case J_RIGHT:
            return THIS->x < mapBoundRight;
        case J_UP:
            return THIS->y > mapBoundUp;
        case J_DOWN:
            return THIS->y < mapBoundDown;
        default:
            return FALSE;
    }
}

static void chooseEnemyDirection(void) {
    const UBYTE tunnel = getEnemyCurrentTunnel();
    const UBYTE currentDirection = THIS->custom_data[enemy_direction];
    const UBYTE reverseDirection = oppositeDirectionBit(currentDirection);
    UBYTE priorities[3];
    UBYTE idx;

    if (enemyCanMove(currentDirection, tunnel)) {
        return;
    }

    if (currentDirection == J_LEFT || currentDirection == J_RIGHT) {
        priorities[0] = J_DOWN;
        priorities[1] = J_UP;
    } else {
        priorities[0] = J_LEFT;
        priorities[1] = J_RIGHT;
    }
    priorities[2] = reverseDirection;

    for (idx = 0; idx != 3; ++idx) {
        if (enemyCanMove(priorities[idx], tunnel)) {
            THIS->custom_data[enemy_direction] = priorities[idx];
            THIS->custom_data[movement_accumulator] = 0;
            break;
        }
    }

    // idx == 2 means reverse was the only legal fallback, idx == 3 means blocked.
    if (THIS->custom_data[mode] == nobMode &&
        THIS->custom_data[mode_timer] < 255 &&
        // idx bigger than 1 means the nob had to change direction or is completely stuck
        idx > 1) {
        THIS->custom_data[mode_timer]++;
        if (THIS->custom_data[mode_timer] > (10 - difficultyLevel)) {
            setEnemyMode(hobMode);
        }
    }
}

void START(void) {
    setEnemyMode(waitMode);
    THIS->custom_data[enemy_direction] = J_LEFT;
    THIS->custom_data[movement_accumulator] = 20;
    THIS->lim_x = 256;
    THIS->lim_y = 256;
}



void UPDATE(void) {
    if (isDying == 1) {
        return;
    }
    if (THIS->custom_data[mode] == deadMode) {
        if (THIS->custom_data[mode_timer] > 0) {
            THIS->custom_data[mode_timer]--;
        } else {
            SpriteManagerRemoveSprite(THIS);
        }
        return;
    }
    if (THIS->custom_data[mode] == crushedMode) {
        if (followCrushingBag()) {
            return;
        }
        setEnemyMode(deadMode);
        updateScore(scoreKill);
        return;
    }
    if (THIS->custom_data[mode] == waitMode) {
        if (THIS->custom_data[mode_timer] > 0) {
            THIS->custom_data[mode_timer]--;
        } else {
            setEnemyMode(nobMode);
        }
    }

    if (THIS->custom_data[mode] != nobMode && THIS->custom_data[mode] != hobMode) {
        return;
    }
    if (THIS->custom_data[mode] == hobMode && THIS->custom_data[mode_timer] < 255) {
        THIS->custom_data[mode_timer]++;
    }
    if (isEnemyAligned()) {
        if (THIS->custom_data[mode] == hobMode && THIS->custom_data[mode_timer] > (30 + (difficultyLevel << 1))) {
            setEnemyMode(nobMode);
        }
        chooseEnemyDirection();
    }
    THIS->custom_data[movement_accumulator] += 4;
    if (THIS->custom_data[movement_accumulator] < 25) {
        return;
    }
    THIS->custom_data[movement_accumulator] -= 5;
    switch (THIS->custom_data[enemy_direction]) {
        case J_LEFT:
            if (THIS->x > mapBoundLeft) {
                THIS->x--;
            }
        break;
        case J_RIGHT:
            if (THIS->x < mapBoundRight) {
                THIS->x++;
            }
        break;
        case J_UP:
            if (THIS->y > mapBoundUp) {
                THIS->y--;
            }
        break;
        case J_DOWN:
            if (THIS->y < mapBoundDown) {
                THIS->y++;
            }
        break;
    }
}

void DESTROY(void) {
    enemyCountOnScreen--;
    if (spawnTimer == 0) {
        spawnTimer = getEnemySpawnGapTimer();
    }
}
