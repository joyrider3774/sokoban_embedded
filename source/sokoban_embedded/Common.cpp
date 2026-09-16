#include <stdint.h>
#include "Common.h"
#include "Defines.h"
#include "CWorldParts.h"
#include "CLevelPackFile.h"

uint16_t ColorWhite, ColorBlack;
uint8_t GameState = GSTitleScreenInit;
uint8_t Selection=0,InstalledLevelPacksCount=0;
uint16_t InstalledLevels=0,UnlockedLevels=1;
int16_t SelectedLevel=0;
int8_t SelectedLevelPack=0;
int8_t skin=0;
CWorldParts* WorldParts;
CLevelPackFile *LevelPackFile;
char LevelPackName[MaxLevelPackNameLength];
char InstalledLevelPacks[MaxLevelPacks][MaxLevelPackNameLength];
const uint8_t* IMGBackground, *IMGBox, *IMGFloor, *IMGPlayer, *IMGSpot, *IMGTitleScreen, *IMGWall;
int8_t AskingQuestionID = -1;
bool AskingQuestion = false;
char NormalCreateName[MaxLevelPackNameLength];
uint32_t framecount = 0, frameUpStart = 0, frameDownStart = 0, frameLeftStart = 0, frameRightStart = 0;
uint8_t currButtons, prevButtons;
bool debugMode = false;