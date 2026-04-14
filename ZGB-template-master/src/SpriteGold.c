#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"
#include "SpriteEnemy.h"
#include "StateGame.h"

#define goldCrumbleTime (5 * originalTickToGameBoyFrameRatio)
#define goldFinalWindowFrames (10 * originalTickToGameBoyFrameRatio)

#define stateCrumbling 1
#define stateStatic 2

#define goldStatus 0
#define goldTimerLo 1
#define goldTimerHi 2


const UBYTE gold_crumbling[] = {1, 0};
const UBYTE gold_static[] = {1, 0};

// CUSTOM_DATA usage
// 0 state
// 1-2 16-bit timer

static uint16_t getGoldTimer(void) {
    return ((uint16_t)THIS->custom_data[goldTimerHi] << 8) | THIS->custom_data[goldTimerLo];
}

static void setGoldTimer(uint16_t timer) {
    THIS->custom_data[goldTimerLo] = (UBYTE)timer;
    THIS->custom_data[goldTimerHi] = (UBYTE)(timer >> 8);
}

static uint16_t getGoldLifetimeFrames(void) {
    UBYTE level = currentLevel;
    if (level == 0) {
        level = 1;
    }
    return (uint16_t)(150 - (level * 10)) * originalTickToGameBoyFrameRatio;
}

static BOOLEAN goldShouldExpireSoon(void) {
    UBYTE currentColumn;
    UBYTE currentRow;
    UBYTE cellBelow;

    if (THIS->y >= mapBoundDown) {
        return FALSE;
    }

    currentColumn = LARGE_TILE_FROM_PIXEL(THIS->x - mapBoundLeft);
    currentRow = LARGE_TILE_FROM_PIXEL(THIS->y - mapBoundUp);
    if (currentRow >= mapMetaHeight - 1) {
        return FALSE;
    }

    cellBelow = currentRow * mapMetaWidth + currentColumn + mapMetaWidth;
    return itemMap[cellBelow] != itemBag &&
        tunnelMap[cellBelow] != 0;
}

void START(void) {
    if(_cpu != CGB_TYPE){
        SPRITE_SET_PALETTE(THIS,1);
    }
    THIS->custom_data[goldStatus] = stateCrumbling;
    setGoldTimer(goldCrumbleTime);
    SetSpriteAnim(THIS, gold_crumbling, 10);
    THIS->lim_x = 256;
    THIS->lim_y = 256;
}

void UPDATE(void) {
    uint8_t state = THIS->custom_data[goldStatus];
    uint16_t timer = getGoldTimer();

    if (isDying || paused) {
        return;
    }

    if (state == stateCrumbling) {
        if (timer == 0) {
            const UBYTE cell = getMapMetaTileArrayPosition(THIS->x, THIS->y);
            THIS->custom_data[goldStatus] = stateStatic;
            itemMap[cell] = itemGold;
            renderMetaCell(cell);
            setGoldTimer(getGoldLifetimeFrames());
            SetSpriteAnim(THIS, gold_static, 10);
            SetVisible(THIS, FALSE);
        } else {
            setGoldTimer(timer - 1);
        }
        return;
    }

    if (itemMap[getMapMetaTileArrayPosition(THIS->x, THIS->y)] != itemGold) {
        SpriteManagerRemoveSprite(THIS);
        return;
    }

    if (goldShouldExpireSoon() && timer > goldFinalWindowFrames) {
        timer = goldFinalWindowFrames;
        setGoldTimer(timer);
    }

    if (timer == 0) {
        SpriteManagerRemoveSprite(THIS);
    } else {
        setGoldTimer(timer - 1);
    }
}

void DESTROY(void) {
    const UBYTE cell = getMapMetaTileArrayPosition(THIS->x, THIS->y);

    if (itemMap[cell] == itemGold) {
        itemMap[cell] = itemNone;
        renderMetaCell(cell);
    }
}
