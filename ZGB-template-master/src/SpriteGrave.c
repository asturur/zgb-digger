#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"
#include "StateGame.h"
#include "SpriteGrave.h"

const UBYTE grave_emerge[] = {5, 0, 1, 2, 3, 4, 5};

void START(void) {
    SetSpriteAnim(THIS, grave_emerge, 10);
    THIS->lim_x = 256;
    THIS->lim_y = 256;
}

void UPDATE(void) {

}

void DESTROY(void) {

}
