#ifndef CWORLDPARTS_H
#define CWORLDPARTS_H

#include <stdint.h>
#include "Defines.h"
#include "CViewPort.h"
#include "CLevelPackFile.h"

//ItemCount goes up to MAXWORLDPARTS, MoveAbleItemCount up to MAXMOVEABLEWORLDPARTS
static_assert(MAXWORLDPARTS <= 65535, "ItemCount does not fit in uint16_t");
static_assert(MAXMOVEABLEWORLDPARTS <= 255, "MoveAbleItemCount does not fit in uint8_t");

typedef struct CWorldPart CWorldPart;
typedef struct CWorldParts CWorldParts;
//fields are ordered big to small to avoid padding
struct CWorldParts
{
	CViewPort *ViewPort;
	CWorldPart *Items[MAXWORLDPARTS];
	CWorldPart* MoveAbleItems[MAXMOVEABLEWORLDPARTS];
	CWorldPart *Player;
	//no upper bound, a player that keeps walking in one level passes 65535 moves in
	//about two and a half hours, 32 bits can not run out. Undo only takes back a move
	//it counted, but it takes Pushes below 0 for a box that was placed with
	//CWorldPart_SetPosition (CWorldParts_CenterLevel) instead of being pushed
	uint32_t Moves;
	int32_t Pushes;
	uint16_t ItemCount;
	uint8_t MoveAbleItemCount;
	bool isLevelPackFileLevel;
	bool DisableSorting;
};

CWorldParts* CWorldParts_Create();
void CWorldParts_Sort(CWorldParts* WorldParts);

void CWorldParts_Add(CWorldParts* WorldParts, CWorldPart *WorldPart);
bool CWorldParts_CenterLevel(CWorldParts* WorldParts);
void CWorldParts_CenterVPOnPlayer(CWorldParts* WorldParts);
void CWorldParts_LimitVPLevel(CWorldParts* WorldParts);
void CWorldParts_Move(CWorldParts* WorldParts);
void CWorldParts_HistoryAdd(CWorldParts* WorldParts);
void CWorldParts_HistoryGoBack(CWorldParts* WorldParts);
void CWorldParts_Draw(CWorldParts* WorldParts);
//repaint only the 8x8 screen cells whose contents changed
bool CWorldParts_DrawBoard(CWorldParts* WorldParts);
void CWorldParts_MarkDirty(int16_t x, int16_t y, int16_t w, int16_t h);
void CWorldParts_MarkAllDirty();
void CWorldParts_Remove(CWorldParts* WorldParts, int8_t PlayFieldXin,int8_t PlayFieldYin);
void CWorldParts_Remove_Type(CWorldParts* WorldParts, int8_t PlayFieldXin,int8_t PlayFieldYin,uint8_t Type);
void CWorldParts_RemoveAll(CWorldParts* WorldParts);
bool CWorldParts_LoadFromLevelPackFile(CWorldParts* WorldParts, CLevelPackFile* LPFile, int16_t level, bool doCenterLevel);
bool CWorldParts_ItemExists(CWorldParts* WorldParts, int8_t PlayFieldXin,int8_t PlayFieldYin, uint8_t Type);
void CWorldParts_Destroy(CWorldParts* WorldParts);
#if FLOODFILLFLOOR
void CWorldParts_DrawFloor(CWorldParts* WorldParts, CWorldPart* Player);
#endif


#endif