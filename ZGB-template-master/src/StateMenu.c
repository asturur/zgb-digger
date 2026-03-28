#include "Banks/SetAutoBank.h"

#include "Scroll.h"
#include "Keys.h"
#include "ZGBMain.h"

IMPORT_MAP(menu_bg);

void START(void) {
	InitScroll(BANK(menu_bg), &menu_bg, 0, 0);
	MoveScroll(0, 0);
}

void UPDATE(void) {
	if(KEY_TICKED(J_START)) {
		SetState(StateGame);
	}
	if(KEY_TICKED(J_SELECT)) {
		SetState(StateOptions);
	}
}

void DESTROY(void) {
}
