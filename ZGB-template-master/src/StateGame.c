#include "Banks/SetAutoBank.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"

IMPORT_MAP(map);

void START() {
	scroll_target = SpriteManagerAdd(SpritePlayer, 136, 168);
	InitScroll(BANK(map), &map, 0, 0);
}

void UPDATE() {
	
}
