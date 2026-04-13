
#include "Sprite.h"

#define waitMode 0
#define nobMode 1
#define hobMode 2
#define deadMode 3
#define crushedMode 4
#define scoreboardNobMode 5
#define scoreboardHobMode 6

#define changeTimer 255
#define deathTimer 45
#define initialWaitTime 20 // 5 tick * 4

// CUSTOM_DATA usage
#define mode 0
#define mode_timer 1
// #define timerQty 2
#define enemy_direction 3
#define frightned 4
#define enemy_movement_accumulator 6
// #define deathTimer 7

#define enemySpawnGapBaseTimer 180
#define enemySpawnGapDifficultyStep 8

void crushEnemy(Sprite* enemy) BANKED;
UBYTE killEnemy(Sprite* enemy) BANKED;
void setEnemyModeFor(Sprite* enemy, UBYTE enemyMode) BANKED;
