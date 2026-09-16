#ifndef CLEVELPACKFILE_H
#define CLEVELPACKFILE_H

#include <stdint.h>

#include "Defines.h"

#define MAXLEVELS 1000
#define MAXITEMCOUNT (NrOfCols*NrOfRows) + 2
#define MAXCOMMENTLEN 10
#define MAXSETLEN 50
#define MAXAUTHORLEN 25
#define MAXTITLELEN 35
#define MAXLEVELFIELDDATALEN 35
#define MAXLEVELFIELDLEN 10
#define MAXLINELEN 90

//values for the level parameter of the parse / load functions, any value >= 1
//means parse the whole pack but only keep that single level in memory
#define LPLevelHeaderOnly -1 //only read the pack's set / author, stop at the first level
#define LPLevelCountOnly 0   //read the whole pack to count its levels, keep no level

#define LPWall 35          //'#' 
#define LPSpot 46          //'.'
#define LPPlayer 64        //'@'
#define LPBox 36           //'$'
#define LPPlayerOnSpot 43  //'+'
#define LPBoxOnSpot 42     //'*'
#define LPFloor 32         //' '

//level numbers are int16_t (-1 = LPLevelHeaderOnly), part counts uint16_t, a column
//is a position in a line so it fits a uint8_t, and so does the playfield size
static_assert(MAXLEVELS <= 32767, "level numbers do not fit in int16_t");
static_assert(MAXITEMCOUNT <= 65535, "part counts do not fit in uint16_t");
static_assert((MAXLINELEN <= 256) && (NrOfCols <= 255) && (NrOfRows <= 255), "line positions do not fit in uint8_t");

typedef struct LevelPart LevelPart;
//an id and a tile coordinate each fit in a byte, at MAXITEMCOUNT parts the
//difference between this and three ints is over 3k of heap
struct LevelPart
{
	uint8_t id;
	uint8_t x;
	uint8_t y;
};

typedef struct LevelMeta LevelMeta;
struct LevelMeta
{
	uint8_t minx;    //a column (below MAXLINELEN) or NrOfCols
	//rows are counted per line of the level text, which a malformed pack can make
	//longer than 255 lines. maxx is set to 1000 to reject a level with too many parts
	uint16_t miny;
	uint16_t maxx;
	uint16_t maxy;
	uint16_t parts;  //below MAXITEMCOUNT
	char author[MAXAUTHORLEN];
	char comments[MAXCOMMENTLEN];
	char title[MAXTITLELEN];
};

typedef struct CLevelPackFile CLevelPackFile;
struct CLevelPackFile
{
	//only the level that is currently selected is kept in memory, the pack is
	//reparsed from scratch whenever another level is requested
	LevelPart Level[MAXITEMCOUNT];
	LevelMeta Meta;
	char author[MAXAUTHORLEN];
	char set[MAXSETLEN];
	char filename[MaxLevelPackNameLength];
	uint16_t LevelCount;  //0 .. MAXLEVELS
	uint16_t LoadedLevel; //1 based number of the level held in Level / Meta, 0 = none
	bool Loaded;
};

CLevelPackFile* CLevelPackFile_Create();
void CLevelPackFile_Destroy(CLevelPackFile* LevelPackFile);
//a pack's text is longer than 65535 bytes (696.sok is 69250)
bool CLevelPackFile_parseText(CLevelPackFile* LevelPackFile, const unsigned char* text, uint32_t textLen, uint8_t maxWidth, uint8_t maxHeight, int16_t level);
bool CLevelPackFile_loadFile(CLevelPackFile* LevelPackFile, char* filename, uint8_t maxWidth, uint8_t maxHeight, int16_t level);
bool CLevelPackFile_loadLevel(CLevelPackFile* LevelPackFile, int16_t level);
//the level packs LEVELPACKS builds in: how many there are and the file name of each
uint8_t CLevelPackFile_BuiltInCount(void);
const char* CLevelPackFile_BuiltInName(uint8_t index);

#endif