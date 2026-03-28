#include "Banks/SetAutoBank.h"

#include "Scroll.h"
#include "Keys.h"
#include "Print.h"
#include "ZGBMain.h"

#define CENTER(len) (((SCREEN_TILES_W - (len)) >> 1) + 1)

#define menuStart 1
#define menuLength 2
#define menuCursorFontTile 46

extern BOOLEAN infiniteLives;
extern BOOLEAN invincibility;

IMPORT_TILES(font);

UBYTE menu_pos_y = menuStart;

static void moveMenuDown(void) {
    if (menu_pos_y < menuStart + menuLength - 1) {
        set_bkg_tile_xy(SCREEN_TILES_W - 4, menu_pos_y, 0);
        menu_pos_y++;
        set_bkg_tile_xy(SCREEN_TILES_W - 4, menu_pos_y, menuCursorFontTile);
    }
}

static void moveMenuUp(void) {
    if (menu_pos_y > menuStart) {
        set_bkg_tile_xy(SCREEN_TILES_W - 4, menu_pos_y, 0);
        menu_pos_y--;
        set_bkg_tile_xy(SCREEN_TILES_W - 4, menu_pos_y, menuCursorFontTile);
    }
}

static void toggleMenu(void) {
    switch (menu_pos_y) {
        case 1:
            infiniteLives = !infiniteLives;
            PRINT(SCREEN_TILES_W - 3,  1, infiniteLives ? "ON ": "OFF ");
            break;
        case 2:
            invincibility = !invincibility;
            PRINT(SCREEN_TILES_W - 3,  2, invincibility ? "ON ": "OFF ");
            break;
        default:
            break;
    }
}

void START(void) {
	move_bkg(0, 0);
	INIT_FONT(font, PRINT_BKG);
    fill_bkg_rect(0, 0, 20, 18, 0);

	PRINT(CENTER(7),  0, "OPTIONS");

    PRINT(0,  1, "INFINITE LIVES");
    
    PRINT(SCREEN_TILES_W - 3,  1, "OFF");

    PRINT(0,  2, "INVINCIBILITY");
    PRINT(SCREEN_TILES_W - 3,  2, "OFF");

	PRINT(0, 17, "PRESS START TO EXIT");
    set_bkg_tile_xy(SCREEN_TILES_W - 4, menu_pos_y, menuCursorFontTile);
}

void UPDATE(void) {
	if(KEY_TICKED(J_START)) {
		SetState(StateMenu);
	}
    if(KEY_TICKED(J_DOWN)) {
		moveMenuDown();
	}
    if(KEY_TICKED(J_UP)) {
		moveMenuUp();
	}
    if(KEY_TICKED(J_A)) {
		toggleMenu();
	}
}

void DESTROY(void) {
}
