#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include "Common.h"
#include "Defines.h"
#include "CWorldParts.h"
#include "CLevelPackFile.h"
#include "Platform.h"

extern uint16_t ColorWhite, ColorBlack;
//sized to what they hold: GameState at most GSOptionsInit, Selection a menu entry (1 .. 4),
//pack counts at most MaxLevelPacks and level numbers at most MAXLEVELS. SelectedLevelPack
//goes to -1 before it wraps, AskingQuestionID uses -1 for none and SelectedLevel is
//decremented before it is clamped to 1
static_assert((GSOptionsInit <= 255) && (MaxLevelPacks <= 127) && (MAXLEVELS <= 32767) && (IDQuitPlaying <= 127), "globals do not fit their types");
extern uint8_t GameState;
extern uint8_t Selection,InstalledLevelPacksCount;
extern uint16_t InstalledLevels,UnlockedLevels;
extern int16_t SelectedLevel;
extern int8_t SelectedLevelPack;
extern CWorldParts* WorldParts;
extern CLevelPackFile *LevelPackFile;
extern char LevelPackName[MaxLevelPackNameLength];
extern char InstalledLevelPacks[MaxLevelPacks][MaxLevelPackNameLength];
extern int8_t AskingQuestionID;
extern bool AskingQuestion;
extern const uint8_t* IMGBackground, *IMGBox, *IMGFloor, *IMGPlayer, *IMGSpot, *IMGTitleScreen, *IMGWall;
extern char NormalCreateName[MaxLevelPackNameLength];
extern uint8_t currButtons, prevButtons;
//frame counters that never stop, the menus only use differences of them
extern uint32_t framecount, frameUpStart, frameDownStart, frameLeftStart, frameRightStart;
extern bool debugMode;
extern int8_t skin;
#endif