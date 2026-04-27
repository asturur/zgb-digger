#include "Banks/SetAutoBank.h"

#include <gb/gb.h>
#include "Keys.h"
#include "Print.h"
#include "Scroll.h"
#include "ZGBMain.h"
#include "savegame.h"


#define CENTER(len) ((SCREEN_TILES_W - (len)) >> 1)
#define usedLetters 43
#define screenFlashTimerValue 10
#define numSavedHiscores 10

IMPORT_TILES(font);

extern Savegame savegame;
extern uint32_t score;

static UBYTE currentIndex = 0;
static UBYTE screenFlashTimer = screenFlashTimerValue;
static BOOLEAN charVisible = TRUE;

uint8_t initials[4] = {'-', '-', '-', 0};
uint8_t alphaIndex[3] = {0 ,0 ,0};
uint8_t alphabet[usedLetters] = {'-', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '!', '(', ')', '.', ':', '?'};
uint8_t newSpot = 10;

static void printInitials() {
    uint8_t initial1[2] = {currentIndex == 0 && charVisible == FALSE ? ' ' :  initials[0], 0};
    uint8_t initial2[2] = {currentIndex == 1 && charVisible == FALSE ? ' ' :  initials[1], 0};
    uint8_t initial3[2] = {currentIndex == 2 && charVisible == FALSE ? ' ' :  initials[2], 0};
    PRINT(CENTER(5),  10, "%s %s %s", &initial1, &initial2, &initial3);
}

void printScoreOnScreen(uint32_t scoreToPrint, uint8_t posX, uint8_t posY) BANKED {
	// scores are multiple of 25 points.
	// mod 2 = 1 draw a 5.
	// then mod 4 for 2/5/7
	// then >> 2 will give us the hundreds.
	// then we need to loop again
	uint8_t mod = 0;
	uint32_t lastScore = scoreToPrint;
    uint8_t printScore[7] = {' ', ' ', ' ', ' ', ' ', ' ', 0};
	for (uint8_t i = 6; i > 0; i--) {
		mod = lastScore % 10;
        printScore[i - 1] = (uint8_t)'0' + mod;
		lastScore = (lastScore - mod) / 10;
	}
    PRINT(posX, posY, "%s", printScore);
}

void START(void) {

    ENABLE_RAM;
    for (uint8_t i = 0; i < numSavedHiscores; i++) {
        if (savegame.hiscores[i].score == UINT32_MAX || savegame.hiscores[i].score < score) {
            newSpot = i;
            break;
        }
    }
    DISABLE_RAM;

    if (newSpot > numSavedHiscores - 1) {
        SetState(StateMenu);
        return;
    }

    move_bkg(0, 0);
	INIT_FONT(font, PRINT_BKG);
    fill_bkg_rect(0, 0, 20, 18, 0);

	PRINT(CENTER(8),  0, "PLAYER 1");

	PRINT(CENTER(14),  2, "NEW HIGH SCORE");
    printScoreOnScreen(score, CENTER(8),  4);

    PRINT(CENTER(10),  6, "ENTER YOUR");
    PRINT(CENTER(8),  7, "INITIALS");

    printInitials();

	PRINT(0, 17, "A-CONFIRM     B-BACK");
}

void UPDATE(void) {

    if(screenFlashTimer > 0) {
        screenFlashTimer--;
    } else {
        charVisible = !charVisible;
        screenFlashTimer = screenFlashTimerValue;
        printInitials();
    }

    if((KEY_TICKED(J_RIGHT) | KEY_TICKED(J_A)) && currentIndex < 2) {
        currentIndex++;
        printInitials();
        return;
    }
    if(KEY_TICKED(J_LEFT) && currentIndex > 0) {
        currentIndex--;
        printInitials();
        return;
    }
    if(KEY_TICKED(J_UP)) {
        if (alphaIndex[currentIndex] < usedLetters) {
            alphaIndex[currentIndex]++;
        } else {
            alphaIndex[currentIndex] = 0;
        }
        initials[currentIndex] = alphabet[alphaIndex[currentIndex]];
        printInitials();
        return;
    }
    if(KEY_TICKED(J_DOWN)) {
        if (alphaIndex[currentIndex] > 0) {
            alphaIndex[currentIndex]--;
        } else {
            alphaIndex[currentIndex] = usedLetters;
        }
        initials[currentIndex] = alphabet[alphaIndex[currentIndex]];
        printInitials();
        return;
    }    
	if(KEY_TICKED(J_B)) {
        if (initials[currentIndex] == alphabet[0]) {
            if (currentIndex > 0) {
                currentIndex--;
            }
        } else {
            alphaIndex[currentIndex] = 0;
            initials[currentIndex] = alphabet[alphaIndex[currentIndex]];
        }
        printInitials();
        return;
        // erase current, then moveback.
	}
	if(KEY_TICKED(J_A) && currentIndex == 2) {
        ENABLE_RAM;
        for (uint8_t i = numSavedHiscores - 2; i >= newSpot; i--) {
            savegame.hiscores[i+1].score = savegame.hiscores[i].score;
            memcpy(&savegame.hiscores[i+1].initials, &savegame.hiscores[i].initials, 4);
        }
        savegame.hiscores[newSpot].score = score;
        memcpy(&savegame.hiscores[newSpot].initials, &initials, 4);
        DISABLE_RAM;
        // end insert
        SetState(StateScoreboard);
        return;
	}
}

void DESTROY(void) {
}
