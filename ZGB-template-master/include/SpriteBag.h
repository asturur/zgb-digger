#include "Sprite.h"

#define shakeBeforeFall 60
#define bagActivationGraceFrames 20

#define stateStatic 1
#define stateShaking 2
#define stateFalling 3
#define statePushing 4

#define bagStatus 0
#define bagStateTimer 1
#define bagFallCounter 2
#define bagDirection 3
#define bagPushDistance 4
#define bagMovementAccumulator 5

// tiles numbers for bag on black
#define bagTL 22
#define bagTR 23
#define bagBL 24
#define bagBR 25
#define bagOnGrass FALSE
#define bagOnTunnel TRUE

void setBagState(Sprite* bag, UBYTE bagState) BANKED;
void restoreStaticBag(Sprite* bag) BANKED;
