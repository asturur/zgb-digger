#ifndef ZGBMAIN_H
#define ZGBMAIN_H

#define STATES \
_STATE(StateSplashZGB)\
_STATE(StateMenu)\
_STATE(StateScoreboard)\
_STATE(StateOptions)\
_STATE(StateGame)\
STATE_DEF_END

#define SPRITES \
_SPRITE_DMG(SpritePlayer, player)\
_SPRITE_DMG(SpriteEnemy, hobnob)\
_SPRITE_DMG(SpriteBag, bag)\
_SPRITE_DMG(SpriteFireball, fireball)\
_SPRITE_DMG(SpriteGold, gold)\
_SPRITE_DMG(SpriteGrave, grave)\
SPRITE_DEF_END

#include "ZGBMain_Init.h"

#endif
