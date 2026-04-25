#include "Banks/SetAutoBank.h"

#include <gb/gb.h>
#include "Keys.h"
#include "Print.h"
#include "Scroll.h"
#include "ZGBMain.h"
#include "savegame.h"


#define CENTER(len) ((SCREEN_TILES_W - (len)) >> 1)
#define usedLetters 43

IMPORT_TILES(font);

extern Savegame savegame;
extern uint16_t score;

static UBYTE currentIndex = 0;

uint8_t initials[4] = {'-', '-', '-', 0};
uint8_t alphaIndex[3] = {0 ,0 ,0};
uint8_t alphabet[usedLetters] = {'-', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '!', '(', ')', '.', ':', '?'};

static void printInitials() {
    uint8_t initial1[2] = {initials[0], 0};
    uint8_t initial2[2] = {initials[1], 0};
    uint8_t initial3[2] = {initials[2], 0};
    PRINT(CENTER(5),  9, "%s %s %s", &initial1, &initial2, &initial3);
}

void START(void) {

    move_bkg(0, 0);
	INIT_FONT(font, PRINT_BKG);
    fill_bkg_rect(0, 0, 20, 18, 0);

	PRINT(CENTER(8),  0, "PLAYER 1");

	PRINT(CENTER(14),  2, "NEW HIGH SCORE");
    PRINT(CENTER(6),  3, "000000");

    PRINT(CENTER(10),  5, "ENTER YOUR");
    PRINT(CENTER(8),  6, "INITIALS");

    printInitials();

	PRINT(0, 17, "A-CONFIRM     B-BACK");
}

void UPDATE(void) {
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
        savegame.hiscores[0].score = score;
        savegame.hiscores[0].initials[0] = initials[0];
        savegame.hiscores[0].initials[1] = initials[1];
        savegame.hiscores[0].initials[2] = initials[2];
        DISABLE_RAM;
        // end insert
        SetState(StateScoreboard);
        return;
	}
}

void DESTROY(void) {
}
