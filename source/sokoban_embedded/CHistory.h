#ifndef CHISTORY_H
#define CHISTORY_H

#include <stdint.h>
#include "Defines.h"
#include "CWorldPart.h"

//ItemCount is a uint8_t
static_assert(MaxHistory <= 255, "MaxHistory does not fit in uint8_t");

typedef struct CWorldPart CWorldPart;

typedef struct SPrevPoint SPrevPoint;
//a tile coordinate of a spot the part stood on, same range as CWorldPart's
//PlayFieldX / PlayFieldY. MaxHistory of these are kept per moveable part, so two
//bytes instead of eight here is 150 bytes saved on every box in the level
struct SPrevPoint
{
	int8_t X,Y;
};


typedef struct CHistory CHistory;
struct CHistory
{
	CWorldPart *Part;
	SPrevPoint Items[MaxHistory];
	uint8_t ItemCount;             //0 .. MaxHistory
};

CHistory* CHistory_Create(CWorldPart *Partin);
void CHistory_Add (CHistory* History, int8_t X, int8_t Y);
void CHistory_GoBack(CHistory* History);
void CHistory_Destroy(CHistory* History);

#endif