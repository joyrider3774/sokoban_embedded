#ifndef CWORLDPART_H
#define CWORLDPART_H

#include "CHistory.h"
#include "CWorldParts.h"
#include <stdint.h>

//playfield positions are stored as int8_t, CanMoveTo is asked about the tile next to the edge
static_assert((NrOfCols <= 127) && (NrOfRows <= 127), "playfield size does not fit in int8_t");
//pixel positions are stored as int16_t
static_assert((NrOfCols * TileWidth <= 32767) && (NrOfRows * TileHeight <= 32767), "pixel positions do not fit in int16_t");
//a speed is negated into the int8_t Xi / Yi, the viewport is moved by the same int8_t steps
static_assert((GameMoveSpeed <= 127) && (ViewportMove <= 127) && (PlayerAnimDelay <= 255), "move speed, viewport step or anim delay does not fit its type");

typedef struct CWorldParts CWorldParts;
typedef struct CHistory CHistory;

typedef struct CWorldPart CWorldPart;
//every field is sized to the range it actually holds and they are ordered big to
//small so the compiler does not have to pad between them. Parts come out of a
//fixed pool of MAXWORLDPARTS entries, so every byte saved here is saved 402 times
struct CWorldPart
{
	CHistory *History;
	int16_t X,Y;                    //pixel position, up to NrOfCols * TileWidth
	int8_t PlayFieldX,PlayFieldY;   //tile position, signed: centering can put a part outside the walls below 0
	int8_t Xi,Yi;                   //pixel step per frame while moving, -MoveSpeed..MoveSpeed
	int8_t MoveDelayCounter;        //counts up to MoveDelay, reset through -1
	uint8_t MoveSpeed,MoveDelay;    //GameMoveSpeed and 0
	uint8_t Type,Z;                 //IDxxx and Zxxx, 0 .. 6 (Type 0 = free pool slot)
	uint8_t AnimBase,AnimPhase,AnimPhases,AnimCounter,AnimDelay,AnimDelayCounter; //phases 0 .. 15
	uint8_t LastAnimPhase;          //AnimPhase as it was last painted, 0xFF = never
	bool FirstArriveEventFired;
	bool BHistory;
	bool IsMoving;
};

CWorldPart* CWorldPart_Create(const int8_t PlayFieldXin,const int8_t PlayFieldYin,bool CreateHistory, const uint8_t TypeId);

uint8_t CWorldPart_GetType(CWorldPart* WorldPart);
int16_t CWorldPart_GetX(CWorldPart* WorldPart);
int16_t CWorldPart_GetY(CWorldPart* WorldPart);
int8_t CWorldPart_GetPlayFieldX(CWorldPart* WorldPart);
int8_t CWorldPart_GetPlayFieldY(CWorldPart* WorldPart);
uint8_t CWorldPart_GetZ(CWorldPart* WorldPart);
uint8_t CWorldPart_GetAnimPhase(CWorldPart* WorldPart);
bool CWorldPart_HasHistory(CWorldPart* WorldPart);
void CWorldPart_HistoryAdd(CWorldPart* WorldPart);
void CWorldPart_HistoryGoBack(CWorldPart* WorldPart);
void CWorldPart_SetAnimPhase(CWorldPart* WorldPart, uint8_t AnimPhaseIn);
void CWorldPart_SetPosition(CWorldPart* WorldPart, const int8_t PlayFieldXin,const int8_t PlayFieldYin);


void CWorldPart_Event_ArrivedOnNewSpot(CWorldPart* WorldPart);
void CWorldPart_Event_BeforeDraw(CWorldPart* WorldPart);
void CWorldPart_Event_LeaveCurrentSpot(CWorldPart* WorldPart);
void CWorldPart_Event_Moving(CWorldPart* WorldPart, int16_t ScreenPosX,int16_t ScreenPosY,int8_t ScreenXi, int8_t ScreenYi);

void CWorldPart_MoveTo(CWorldPart* WorldPart, const int8_t PlayFieldXin,const int8_t PlayFieldYin,bool BackWards);
bool CWorldPart_CanMoveTo(CWorldPart* WorldPart, const int8_t PlayFieldXin,const int8_t PlayFieldYin);
void CWorldPart_Move(CWorldPart* WorldPart);
void CWorldPart_Draw(CWorldPart* WorldPart);
const uint8_t* CWorldPart_SpriteData(CWorldPart* WorldPart);

void CWorldPart_Destroy(CWorldPart* WorldPart);

#endif