#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <inttypes.h>
#include "CHistory.h"
#include "CWorldPart.h"
#include "Platform.h"

CHistory* CHistory_Create(CWorldPart *Partin)
{
	CHistory* Result = (CHistory*)malloc(sizeof(CHistory));
	//out of heap. Returning NULL lets CWorldPart_Create keep the part and just
	//drop its undo list, writing to Result here would fault on address 0
	if (!Result)
	{
		Platform_Log("CHistory_Create: out of heap, %" PRIu32 " free\n", Platform_FreeHeap());
		return NULL;
	}
	Result->ItemCount = 0;
	Result->Part = Partin;
	return Result;
}

void CHistory_Destroy(CHistory* History)
{
	if(History)
	{
		free(History);
		History = NULL;
	}
}

void CHistory_Add(CHistory* History, int8_t X, int8_t Y)
{
	SPrevPoint Temp;
	uint8_t Teller;
	Temp.X = X;
	Temp.Y = Y;
	if (History->ItemCount < MaxHistory)
	{
		History->Items[History->ItemCount] = Temp;
		History->ItemCount++;
	}
	else
	{
		for (Teller=0;Teller < History->ItemCount - 1 ;Teller++)
		{
			History->Items[Teller] = History->Items[Teller+1];
		}
		History->Items[History->ItemCount-1] = Temp;
	}
}

void CHistory_GoBack(CHistory* History)
{
	if(History->ItemCount > 0)
	{
		History->ItemCount--;
		CWorldPart_MoveTo(History->Part, History->Items[History->ItemCount].X,History->Items[History->ItemCount].Y,true);
	}
}
