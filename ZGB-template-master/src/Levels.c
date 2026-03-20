#pragma bank 255
#include <gb/gb.h>
#include "StateGame.h"
// map descriptors
// 0 grass
// 1-15 walkable tunnel12 is a V,  3 is H, 
// EM diamond
// BG bag

// walkability of tunnels
//
// - 8 -   
// |   |
// 1   2
// |   |
// - 4 -
const void __at(255) __bank_level1Map;

const unsigned char level1Map[] = {
	4,  0,  0,  0, BG,  0,  0,  0,  0,  0,  6,  3,  3,  3,  1, // "S   B     HHHHS", 
   12,  0,  0, EM, EM,  0,  0, EM,  0,  0, 12,  0, BG,  0,  0, // "V  CC  C  V B  "
	5, BG,  0, EM, EM,  0,  0, EM,  0,  0, 12,  0,  0,  0,  0, // "VB CC  C  V    ",
   12,  0,  0, EM, EM, BG,  0, EM, BG,  0, 12,  0, EM, EM, EM, // "V  CCB CB V CCC",
   12,  0,  0, EM, EM,  0,  0, EM,  0,  0, 12,  0, EM, EM, EM, // "V  CC  C  V CCC",
	3,  5,  0, EM, EM,  0,  0, EM,  0,  0, 12,  0, EM, EM, EM, // "HH CC  C  V CCC",
	0, 12,  0,  0,  0,  0, BG,  0, BG,  0, 12,  0,  0,  0,  0, // " V    B B V    ",
   0, 10,  3,  3,  5,  0,  0,  0,  0,  0, 12,  0,  0,  0,  0, // " HHHH     V    ",
   EM,  0,  0,  0, 12,  0,  0,  0,  0,  0, 12,  0,  0,  0, EM, // "C   V     V   C",
   EM, EM,  0,  0, 10,  3,  3,  3,  3,  3,  9,  0,  0, EM, EM, // "CC  HHHHHHH  CC",
};

const void __at(255) __bank_level2Map;

const unsigned char level2Map[] = {
	2,  3,  3,  3,  3,  5,  0,  0, BG,  0, BG,  0,  0,  6,  1, // "SHHHHH  B B  HS", 
   0, EM, EM,  0,  0, 12,  0,  0,  0,  0,  0,  0,  0, 12,  0, // " CC  V       V "
	0, EM, EM,  0,  0, 12,  0, EM, EM, EM, EM, EM,  0, 12,  0, // " CC  V CCCCC V ",
   BG, EM, EM, BG,  0, 12,  0, EM, EM, EM, EM, EM,  0, 12,  0, // "BCCB V CCCCC V ",
   EM, EM, EM, EM,  0, 12,  0,  0,  0,  0,  0,  0,  0, 12,  0, // "CCCC V       V ",
   EM, EM, EM, EM,  0, 12,  0, BG,  0,  0,  6,  3,  3,  9,  0, // "CCCC V B  HHHH ",
	0, EM, EM,  0,  0, 12,  0, EM, EM,  0, 12,  0,  0,  0,  0, // " CC  V CC V    ",
   0, BG, BG,  0,  0, 12, EM, EM, EM, EM, 12,  0, EM, EM,  0, // " BB  VCCCCV CC ",
   EM,  0,  0,  0,  0, 12,  0, EM, EM,  0, 12,  0, EM, EM,  0, // "C    V CC V CC ",
   EM, EM,  0,  0,  0, 10,  3,  3,  3,  3,  9,  0,  0,  0,  0, // "CC   HHHHHH    ",
};

const void __at(255) __bank_level3Map;

const unsigned char level3Map[] = {
	2,  3,  3,  3,  5, BG,  0, BG,  0, BG,  6,  3,  3,  3,  1, // "SHHHHB B BHHHHS",
   EM, EM,  0,  0, 12,  0, EM,  0, EM,  0, 12,  0, BG, BG,  0, // "CC  V C C V BB ",
   EM,  0,  0,  0, 12,  0, EM,  0, EM,  0, 12,  0, EM, EM,  0, // "C   V C C V CC ",
    0, BG, BG,  0, 12,  0, EM,  0, EM,  0, 12, EM, EM, EM, EM, // " BB V C C VCCCC",
   EM, EM, EM, EM, 12,  0, EM,  0, EM,  0, 12, EM, EM, EM, EM, // "CCCCV C C VCCCC",
   EM, EM, EM, EM, 10,  3,  3,  7,  3,  3,  9,  0, EM, EM,  0, // "CCCCHHHHHHH CC ",
	0, EM, EM,  0,  0, EM,  0, 12,  0, EM,  0,  0, EM, EM,  0, // " CC  C V C  CC ",
    0, EM, EM,  0,  0, EM,  0, 12,  0, EM,  0,  0,  0,  0,  0, // " CC  C V C     ",
   EM,  0,  0,  0,  0, EM,  0, 12,  0, EM,  0,  0,  0,  0, EM, // "C    C V C    C",
   EM, EM,  0,  0,  0, EM,  0, 11,  0, EM,  0,  0,  0, EM, EM, // "CC   C H C   CC"
};

const void __at(255) __bank_level4Map;

const unsigned char level4Map[] = {
	2,  5, BG, EM, EM, EM, EM, BG, EM, EM, EM, EM, BG,  6,  1, // "SHBCCCCBCCCCBHS"
   EM, 12,  0,  0, EM, EM, EM, EM, EM, EM, EM,  0,  0, 12, EM, // "CV  CCCCCCC  VC"
   EM, 10,  3,  5,  0, EM, EM, EM, EM, EM,  0,  6,  3,  9, EM, // "CHHH CCCCC HHHC"
   EM,  0,  0, 12,  0,  0, EM, EM, EM,  0,  0, 12,  0,  0, EM, // "C  V  CCC  V  C"
    0,  0,  0, 10,  3,  5,  0, EM,  0,  6,  3,  9,  0,  0,  0, // "   HHH C HHH   "
    0,  0, BG,  0,  0, 12,  0, BG,  0, 12,  0,  0, BG,  0,  0, // "  B  V B V  B  "
    0,  0, EM,  0,  0, 12, EM, EM, EM, 12,  0,  0, EM,  0,  0, // "  C  VCCCV  C  "
    0, EM, EM, EM,  0, 10,  3,  7,  3,  9,  0, EM, EM, EM,  0, // " CCC HHHHH CCC "
   EM, EM, EM, EM, EM,  0, EM, 12, EM,  0, EM, EM, EM, EM, EM, // "CCCCC CVC CCCCC"
   EM, EM, EM, EM, EM,  0, EM,  8, EM,  0, EM, EM, EM, EM, EM  // "CCCCC CHC CCCCC"
};

const void __at(255) __bank_level5Map;

const unsigned char level5Map[] = {
     6,  3,  3,  3,  3,  3,  3,  7,  3,  3,  3,  3,  3,  3,  5, // SHHHHHHHHHHHHHS
    12, BG, EM, EM, EM, EM, BG, 12, EM, EM, EM, EM, EM, EM, 12, // VBCCCCBVCCCCCCV
    12, EM, EM, EM, EM, EM, EM, 12,  0, EM, EM, BG, EM,  0, 12, // VCCCCCCV CCBC V
    12,  0, EM, EM, EM, EM,  0, 12, EM, EM, BG, EM, EM, EM, 12, // V CCCC VCCBCCCV
    12, EM, EM, EM, EM, EM, EM, 12,  0, EM, EM, EM, EM,  0, 12, // VCCCCCCV CCCC V
    12,  0, EM, EM, EM, EM,  0, 12, BG, EM, EM, EM, EM, EM, 12, // V CCCC VBCCCCCV
    12, EM, EM, BG, EM, EM, EM, 12,  0, EM, EM, EM, EM,  0, 12, // VCCBCCCV CCCC V
    12,  0, EM, EM, BG, EM,  0, 12, EM, EM, EM, EM, EM, EM, 12, // V CCBC VCCCCCCV
    12, EM, EM, EM, EM, EM, EM, 12, EM, EM, EM, EM, EM, EM, 12, // VCCCCCCVCCCCCCV
    10,  3,  3,  3,  3,  3,  3, 11,  3,  3,  3,  3,  3,  3,  9, // HHHHHHHHHHHHHHH
};

const void __at(255) __bank_level6Map;

const unsigned char level6Map[] = {
     6,  3,  3,  3,  3,  7,  3,  7,  3,  7,  3,  3,  3,  3,  5, // SHHHHHHHHHHHHHS
    12, EM, BG, EM, EM, 12,  0, 12,  0, 12, EM, EM, BG, EM, 12, // VCBCCV V VCCBCV
    12, EM, EM, EM,  0, 12, BG, 12, BG, 12,  0, EM, EM, EM, 12, // VCCC VBVBV CCCV
    12, EM, EM, EM,  6,  9,  0, 12,  0, 10,  5, EM, EM, EM, 12, // VCCCHH V HHCCCV
    12, EM, EM,  0, 12,  0, EM, 12, EM,  0, 12,  0, EM, EM, 12, // VCC V CVC V CCV
    12, EM, EM,  6,  9,  0, EM, 12, EM,  0, 10,  5, EM, EM, 12, // VCCHH CVC HHCCV
    12, EM,  0, 12,  0, EM, EM, 12, EM, EM,  0, 12,  0, EM, 12, // VC V CCVCC V CV
    12, EM,  6,  9, BG, EM, EM, 12, EM, EM, BG, 10,  5, EM, 12, // VCHHBCCVCCBHHCV
    12, EM, 12, EM, EM, EM, EM, 12, EM, EM, EM, EM, 12, EM, 12, // VCVCCCCVCCCCVCV
    10,  3, 11,  3,  3,  3,  3, 11,  3,  3,  3,  3, 11,  3,  9, // HHHHHHHHHHHHHHH
};

const void __at(255) __bank_level7Map;

const unsigned char level7Map[] = {
     2,  5, EM, EM, EM, EM, EM,  4, EM, EM, EM, EM, EM,  6,  1, // SHCCCCCVCCCCCHS
     0, 12, EM, BG, EM, BG, EM, 12, EM, BG, EM, BG, EM, 12,  0, //  VCBCBCVCBCBCV
    BG, 12, EM, EM, EM, EM, EM, 12, EM, EM, EM, EM, EM, 12, BG, // BVCCCCCVCCCCCVB
    EM, 10,  5, EM, EM, EM, EM, 12, EM, EM, EM, EM,  6,  9, EM, // CHHCCCCVCCCCHHC
    EM, EM, 12,  0, EM, EM, EM, 12, EM, EM, EM,  0, 12, EM, EM, // CCV CCCVCCC VCC
    EM, EM, 10,  3,  5, EM, EM, 12, EM, EM,  6,  3,  9, EM, EM, // CCHHHCCVCCHHHCC
    EM, EM, EM, EM, 12,  0, EM, 12, EM,  0, 12, EM, EM, EM, EM, // CCCCV CVC VCCCC
    EM, EM, EM, EM, 10,  5,  0, 12,  0,  6,  9, EM, EM, EM, EM, // CCCCHH V HHCCCC
    EM, EM, EM, EM, EM, 12,  0, 12,  0, 12, EM, EM, EM, EM, EM, // CCCCCV V VCCCCC
    EM, EM, EM, EM, EM, 10,  3, 11,  3,  9, EM, EM, EM, EM, EM, // CCCCCHHHHHCCCCC
};

const void __at(255) __bank_level8Map;

const unsigned char level8Map[] = {
     6,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  5, // HHHHHHHHHHHHHHS
    12,  0, EM, EM, BG, EM, EM, EM, EM, EM, BG, EM, EM,  0, 12, // V CCBCCCCCBCC V
    14,  3,  5, EM, EM, EM, EM, BG, EM, EM, EM, EM,  6,  3, 13, // HHHCCCCBCCCCHHH
    12, BG, 12,  0, EM, EM, EM, EM, EM, EM, EM,  0, 12, BG, 12, // VBV CCCCCCC VBV
    12, EM, 10,  3,  5, EM, EM, EM, EM, EM,  6,  3,  9, EM, 12, // VCHHHCCCCCHHHCV
    12, EM, EM, BG, 12,  0, EM, EM, EM,  0, 12, BG, EM, EM, 12, // VCCBV CCC VBCCV
    12, EM, EM, EM, 10,  3,  5, EM,  6,  3,  9, EM, EM, EM, 12, // VCCCHHHCHHHCCCV
    12, EM, EM, EM, EM,  0, 12,  0, 12,  0, EM, EM, EM, EM, 12, // VCCCC V V CCCCV
    12, EM, EM, EM, EM, EM, 12,  0, 12, EM, EM, EM, EM, EM, 12, // VCCCCCV VCCCCCV
    10,  3,  3,  3,  3,  3, 11,  3, 11,  3,  3,  3,  3,  3,  9, // HHHHHHHHHHHHHHH
};
