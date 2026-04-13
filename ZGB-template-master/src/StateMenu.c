#include "Banks/SetAutoBank.h"

#include "Scroll.h"
#include "Keys.h"
#include "ZGBMain.h"

IMPORT_MAP(menu_bg);

#define MENU_SCOREBOARD_TIMEOUT_SECONDS 5

UINT16 menu_timer;

void START(void) {
	menu_timer = MENU_SCOREBOARD_TIMEOUT_SECONDS * 60;
	InitScroll(BANK(menu_bg), &menu_bg, 0, 0);
	MoveScroll(0, 0);
}

void UPDATE(void) {
	if(KEY_TICKED(J_START)) {
		SetState(StateGame);
		return;
	}
	if(KEY_TICKED(J_SELECT)) {
		SetState(StateOptions);
		return;
	}
	if(menu_timer > 0 && --menu_timer == 0) {
		SetState(StateScoreboard);
	}
}

void DESTROY(void) {
}
