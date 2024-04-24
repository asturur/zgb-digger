#pragma bank 255
#include <gb/gb.h>

// map descriptors
// 0 grass
// 1-15 walkable tunnel12 is a V,  3 is H, 
// 16 diamond
// 17 bag

// walkability of tunnels
//
// - 8 -   
// |   |
// 1   2
// |   |
// - 4 -
const void __at(255) __bank_level1Map;

const unsigned char level1Map[] = {
	4,  0,  0,  0, 17,  0,  0,  0,  0,  0,  6,  3,  3,  3,  1, // "S   B     HHHHS", 
   12,  0,  0, 16, 16,  0,  0, 16,  0,  0, 12,  0, 17,  0,  0, // "V  CC  C  V B  "
	5, 17,  0, 16, 16,  0,  0, 16,  0,  0, 12,  0,  0,  0,  0, // "VB CC  C  V    ",
   12,  0,  0, 16, 16, 17,  0, 16, 17,  0, 12,  0, 16, 16, 16, // "V  CCB CB V CCC",
   12,  0,  0, 16, 16,  0,  0, 16,  0,  0, 12,  0, 16, 16, 16, // "V  CC  C  V CCC",
	3,  5,  0, 16, 16,  0,  0, 16,  0,  0, 12,  0, 16, 16, 16, // "HH CC  C  V CCC",
	0, 12,  0,  0,  0,  0, 17,  0, 17,  0, 12,  0,  0,  0,  0, // " V    B B V    ",
    0, 10,  3,  3,  5,  0,  0,  0,  0,  0, 12,  0,  0,  0,  0, // " HHHH     V    ",
   16,  0,  0,  0, 12,  0,  0,  0,  0,  0, 12,  0,  0,  0, 16, // "C   V     V   C",
   16, 16,  0,  0, 10,  3,  3,  3,  3,  3,  9,  0,  0, 16, 16, // "CC  HHHHHHH  CC",
};

const void __at(255) __bank_level2Map;

const unsigned char level2Map[] = {
	2,  3,  3,  3,  3,  5,  0,  0, 17,  0, 17,  0,  0,  6,  1, // "SHHHHH  B B  HS", 
    0, 16, 16,  0,  0, 12,  0,  0,  0,  0,  0,  0,  0, 12,  0, // " CC  V       V "
	0, 16, 16,  0,  0, 12,  0, 16, 16, 16, 16, 16,  0, 12,  0, // " CC  V CCCCC V ",
   17, 16, 16, 17,  0, 12,  0, 16, 16, 16, 16, 16,  0, 12,  0, // "BCCB V CCCCC V ",
   16, 16, 16, 16,  0, 12,  0,  0,  0,  0,  0,  0,  0, 12,  0, // "CCCC V       V ",
   16, 16, 16, 16,  0, 12,  0, 17,  0,  0,  6,  3,  3,  9,  0, // "CCCC V B  HHHH ",
	0, 16, 16,  0,  0, 12,  0, 16, 16,  0, 12,  0,  0,  0,  0, // " CC  V CC V    ",
    0, 17, 17,  0,  0, 12, 16, 16, 16, 16, 12,  0, 16, 16,  0, // " BB  VCCCCV CC ",
   16,  0,  0,  0,  0, 12,  0, 16, 16,  0, 12,  0, 16, 16,  0, // "C    V CC V CC ",
   16, 16,  0,  0,  0, 10,  3,  3,  3,  3,  9,  0,  0,  0,  0, // "CC   HHHHHH    ",
};

const void __at(255) __bank_level3Map;

const unsigned char level3Map[] = {
	2,  3,  3,  3,  5, 17,  0, 17,  0, 17,  6,  3,  3,  3,  1, // "SHHHHB B BHHHHS",
   16, 16,  0,  0, 12,  0, 16,  0, 16,  0, 12,  0, 17, 17,  0, // "CC  V C C V BB ",
   16,  0,  0,  0, 12,  0, 16,  0, 16,  0, 12,  0, 16, 16,  0, // "C   V C C V CC ",
    0, 17, 17,  0, 12,  0, 16,  0, 16,  0, 12, 16, 16, 16, 16, // " BB V C C VCCCC",
   16, 16, 16, 16, 12,  0, 16,  0, 16,  0, 12, 16, 16, 16, 16, // "CCCCV C C VCCCC",
   16, 16, 16, 16, 10,  3,  3,  7,  3,  3,  9,  0, 16, 16,  0, // "CCCCHHHHHHH CC ",
	0, 16, 16,  0,  0, 16,  0, 12,  0, 16,  0,  0, 16, 16,  0, // " CC  C V C  CC ",
    0, 16, 16,  0,  0, 16,  0, 12,  0, 16,  0,  0,  0,  0,  0, // " CC  C V C     ",
   16,  0,  0,  0,  0, 16,  0, 12,  0, 16,  0,  0,  0,  0, 16, // "C    C V C    C",
   16, 16,  0,  0,  0, 16,  0, 11,  0, 16,  0,  0,  0, 16, 16, // "CC   C H C   CC"
};

const void __at(255) __bank_level4Map;

const unsigned char level4Map[] = {
	2,  5, 17, 16, 16, 16, 16, 17, 16, 16, 16, 16, 17,  6,  1, // "SHBCCCCBCCCCBHS"
   16, 12,  0,  0, 16, 16, 16, 16, 16, 16, 16,  0,  0, 12, 16, // "CV  CCCCCCC  VC"
   16, 10,  3,  5,  0, 16, 16, 16, 16, 16,  0,  6,  3,  9, 16, // "CHHH CCCCC HHHC"
   16,  0,  0, 12,  0,  0, 16, 16, 16,  0,  0, 12,  0,  0, 16, // "C  V  CCC  V  C"
    0,  0,  0, 10,  3,  5,  0, 16,  0,  6,  3,  9,  0,  0,  0, // "   HHH C HHH   "
    0,  0, 17,  0,  0, 12,  0, 17,  0, 12,  0,  0, 17,  0,  0, // "  B  V B V  B  "
    0,  0, 16,  0,  0, 12, 16, 16, 16, 12,  0,  0, 16,  0,  0, // "  C  VCCCV  C  "
    0, 16, 16, 16,  0, 10,  3,  7,  3,  9,  0, 16, 16, 16,  0, // " CCC HHHHH CCC "
   16, 16, 16, 16, 16,  0, 16, 12, 16,  0, 16, 16, 16, 16, 16, // "CCCCC CVC CCCCC"
   16, 16, 16, 16, 16,  0, 16,  8, 16,  0, 16, 16, 16, 16, 16  // "CCCCC CHC CCCCC"
};

const void __at(255) __bank_level5Map;

const unsigned char level5Map[] = {
     6,  3,  3,  3,  3,  3,  3,  7,  3,  3,  3,  3,  3,  3,  5, // SHHHHHHHHHHHHHS
    12, 17, 16, 16, 16, 16, 17, 12, 16, 16, 16, 16, 16, 16, 12, // VBCCCCBVCCCCCCV
    12, 16, 16, 16, 16, 16, 16, 12,  0, 16, 16, 17, 16,  0, 12, // VCCCCCCV CCBC V
    12,  0, 16, 16, 16, 16,  0, 12, 16, 16, 17, 16, 16, 16, 12, // V CCCC VCCBCCCV
    12, 16, 16, 16, 16, 16, 16, 12,  0, 16, 16, 16, 16,  0, 12, // VCCCCCCV CCCC V
    12,  0, 16, 16, 16, 16,  0, 12, 17, 16, 16, 16, 16, 16, 12, // V CCCC VBCCCCCV
    12, 16, 16, 17, 16, 16, 16, 12,  0, 16, 16, 16, 16,  0, 12, // VCCBCCCV CCCC V
    12,  0, 16, 16, 17, 16,  0, 12, 16, 16, 16, 16, 16, 16, 12, // V CCBC VCCCCCCV
    12, 16, 16, 16, 16, 16, 16, 12, 16, 16, 16, 16, 16, 16, 12, // VCCCCCCVCCCCCCV
    10,  3,  3,  3,  3,  3,  3, 11,  3,  3,  3,  3,  3,  3,  9, // HHHHHHHHHHHHHHH
};


//  {"SHHHHHHHHHHHHHS",
//   "VBCCCCBVCCCCCCV",
//   "VCCCCCCV CCBC V",
//   "V CCCC VCCBCCCV",
//   "VCCCCCCV CCCC V",
//   "V CCCC VBCCCCCV",
//   "VCCBCCCV CCCC V",
//   "V CCBC VCCCCCCV",
//   "VCCCCCCVCCCCCCV",
//   "HHHHHHHHHHHHHHH"},
//  {"SHHHHHHHHHHHHHS",
//   "VCBCCV V VCCBCV",
//   "VCCC VBVBV CCCV",
//   "VCCCHH V HHCCCV",
//   "VCC V CVC V CCV",
//   "VCCHH CVC HHCCV",
//   "VC V CCVCC V CV",
//   "VCHHBCCVCCBHHCV",
//   "VCVCCCCVCCCCVCV",
//   "HHHHHHHHHHHHHHH"},
//  {"SHCCCCCVCCCCCHS",
//   " VCBCBCVCBCBCV ",
//   "BVCCCCCVCCCCCVB",
//   "CHHCCCCVCCCCHHC",
//   "CCV CCCVCCC VCC",
//   "CCHHHCCVCCHHHCC",
//   "CCCCV CVC VCCCC",
//   "CCCCHH V HHCCCC",
//   "CCCCCV V VCCCCC",
//   "CCCCCHHHHHCCCCC"},
//  {"HHHHHHHHHHHHHHS",
//   "V CCBCCCCCBCC V",
//   "HHHCCCCBCCCCHHH",
//   "VBV CCCCCCC VBV",
//   "VCHHHCCCCCHHHCV",
//   "VCCBV CCC VBCCV",
//   "VCCCHHHCHHHCCCV",
//   "VCCCC V V CCCCV",
//   "VCCCCCV VCCCCCV",
//   "HHHHHHHHHHHHHHH"}