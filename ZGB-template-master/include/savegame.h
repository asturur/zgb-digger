//savegame.h
#ifndef SAVEGAME_H
#define SAVEGAME_H

#include <gb/gb.h>
#include "SRAM.h"

typedef struct {
  uint8_t initials[4];
  uint32_t score;
} HiScore;

typedef struct {
    SAVEGAME_HEADER;
    HiScore hiscores[10];
} Savegame;

extern Savegame savegame;

#endif