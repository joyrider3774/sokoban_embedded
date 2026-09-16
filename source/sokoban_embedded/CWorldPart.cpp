#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <inttypes.h>
#include "CWorldParts.h"
#include "CWorldPart.h"
#include "CHistory.h"
#include "Common.h"
#include "Sound.h"
#include "GameFuncs.h"
//parts come out of one fixed pool instead of malloc, that saves the 8 bytes of heap
//overhead every single part would otherwise cost and it can't fragment the heap.
//a free slot is marked by Type 0, no part ever has that id.
//The pool itself is one calloc made when the first part is created, so nothing is
//taken until a level is actually loaded. calloc zeroes it, which marks every slot free
static CWorldPart* WorldPartPool = NULL;
//pool indexes are uint16_t, the search below adds two of them
static_assert(MAXWORLDPARTS * 2 <= 65535, "pool indexes do not fit in uint16_t");
static uint16_t WorldPartPoolNext = 0;

static CWorldPart* CWorldPart_PoolAlloc()
{
	if (!WorldPartPool)
	{
		WorldPartPool = (CWorldPart*)calloc(MAXWORLDPARTS, sizeof(CWorldPart));
		if (!WorldPartPool)
		{
			Platform_Log("CWorldPart_PoolAlloc: out of heap for the part pool, %" PRIu32 " free\n", Platform_FreeHeap());
			return NULL;
		}
		WorldPartPoolNext = 0;
	}
	for (uint16_t Teller = 0; Teller < MAXWORLDPARTS; Teller++)
	{
		//start looking where the last one was taken, allocating a whole level
		//stays linear that way instead of rescanning the pool for every part
		uint16_t Index = WorldPartPoolNext + Teller; //below 2 * MAXWORLDPARTS
		if (Index >= MAXWORLDPARTS)
			Index -= MAXWORLDPARTS;
		if (WorldPartPool[Index].Type == 0)
		{
			WorldPartPoolNext = Index + 1;
			if (WorldPartPoolNext >= MAXWORLDPARTS)
				WorldPartPoolNext = 0;
			return &WorldPartPool[Index];
		}
	}
	return NULL;
}
CWorldPart* CWorldPart_Create(const int8_t PlayFieldXin,const int8_t PlayFieldYin,bool CreateHistory, const uint8_t TypeId)
{
	CWorldPart* Result = CWorldPart_PoolAlloc();
	if (Result)
	{
		Result->BHistory = CreateHistory;
		Result->History = NULL;
		if (Result->BHistory)
		{
			Result->History = CHistory_Create(Result);
			//BHistory gates every access to History, so clearing it leaves a part
			//that still plays normally, it just can not be undone
			if (!Result->History)
				Result->BHistory = false;
		}
		Result->PlayFieldX=PlayFieldXin;
		Result->PlayFieldY=PlayFieldYin;
		Result->Xi=0;
		Result->Yi=0;
		Result->X=PlayFieldXin*TileWidth;
		Result->Y=PlayFieldYin*TileHeight;
		Result->Type=TypeId;
		Result->MoveDelay=0;
		Result->MoveDelayCounter=0;
		Result->IsMoving = false;
		Result->MoveSpeed=0;
		Result->AnimPhase=0;
		Result->LastAnimPhase=0xFF;
		Result->FirstArriveEventFired = false;
		Result->Z=0;
		Result->AnimBase=0;
		Result->AnimPhases=0;
		Result->AnimCounter = 0;
		Result->AnimDelay = 0;
		Result->MoveSpeed = 0;

		switch(Result->Type)
		{
			case IDPlayer:
				Result->AnimBase=4;
				Result->AnimPhase=4;
				Result->AnimPhases=4;
				Result->AnimCounter = 1;
				Result->AnimDelay = PlayerAnimDelay;
				Result->MoveDelay = 0;
				Result->MoveSpeed = GameMoveSpeed;
				Result->AnimDelayCounter =0;
				Result->Z = ZPlayer;
				break;
			case IDSpot:
				Result->Z = ZSpot;
				break;
			case IDBox:
				Result->MoveDelay = 0;
				Result->MoveSpeed = GameMoveSpeed;
				Result->Z = ZBox;
				break;
			case IDWall:
				Result->Z = ZWall;
				break;
			case IDFloor:
				Result->Z = ZFloor;
				break;
			case IDEmpty:
				Result->Z = ZEmpty;
				break;
		}
	}
	return Result;
}

uint8_t CWorldPart_GetType(CWorldPart* WorldPart) 
{
	return WorldPart->Type;
}

int16_t CWorldPart_GetX(CWorldPart* WorldPart) 
{
	return WorldPart->X;
}

int16_t CWorldPart_GetY(CWorldPart* WorldPart) 
{
	return WorldPart->Y;
}

int8_t CWorldPart_GetPlayFieldX(CWorldPart* WorldPart) 
{
	return WorldPart->PlayFieldX;
}

int8_t CWorldPart_GetPlayFieldY(CWorldPart* WorldPart) 
{
	return WorldPart->PlayFieldY;
}

uint8_t CWorldPart_GetZ(CWorldPart* WorldPart) 
{
	return WorldPart->Z;
}

uint8_t CWorldPart_GetAnimPhase(CWorldPart* WorldPart) 
{
	return WorldPart->AnimPhase;
}

bool CWorldPart_HasHistory(CWorldPart* WorldPart) 
{ 
	return WorldPart->BHistory;
}

void CWorldPart_HistoryAdd(CWorldPart* WorldPart) 
{ 
	if (WorldPart->BHistory) 
		CHistory_Add(WorldPart->History,WorldPart->PlayFieldX,WorldPart->PlayFieldY); 
}

void CWorldPart_HistoryGoBack(CWorldPart* WorldPart) 
{ 
	if(WorldPart->BHistory) 
		CHistory_GoBack(WorldPart->History);
}

void CWorldPart_SetAnimPhase(CWorldPart* WorldPart, uint8_t AnimPhaseIn) 
{
	WorldPart->AnimPhase = AnimPhaseIn;
}

void CWorldPart_Event_ArrivedOnNewSpot(CWorldPart* WorldPart) 
{
	uint16_t Teller;
	
	if (WorldPart->Type == IDBox)
	{
		WorldPart->AnimPhase = 0;
		if (WorldParts)
		{
			for (Teller=0;Teller< WorldParts->ItemCount;Teller++)
			{
				if( WorldParts->Items[Teller]->Type == IDSpot)
				{
					if ((WorldParts->Items[Teller]->PlayFieldX == WorldPart->PlayFieldX) && (WorldParts->Items[Teller]->PlayFieldY == WorldPart->PlayFieldY))
					{
						WorldPart->AnimPhase = 1;
						break;
					}
				}
			}
		}
	}

	if(WorldPart->Type == IDSpot)
	{
		if (WorldParts)
		{
			for (Teller=0;Teller< WorldParts->ItemCount;Teller++)
			{
				if( WorldParts->Items[Teller]->Type == IDBox)
				{
					if ((WorldParts->Items[Teller]->PlayFieldX == WorldPart->PlayFieldX) && (WorldParts->Items[Teller]->PlayFieldY == WorldPart->PlayFieldY))
					{
						WorldParts->Items[Teller]->AnimPhase = 1;
						break;
					}
				}
			}
		}
	}
}

void CWorldPart_Event_BeforeDraw(CWorldPart* WorldPart) 
{
	if(WorldPart->Type == IDPlayer)
	{
		if (WorldPart->IsMoving)
		{
			WorldPart->AnimPhase = WorldPart->AnimBase + WorldPart->AnimCounter;
			WorldPart->AnimDelayCounter++;
			if (WorldPart->AnimDelayCounter >= WorldPart->AnimDelay)
			{
				WorldPart->AnimDelayCounter = 0;
				WorldPart->AnimCounter++;
				if (WorldPart->AnimCounter >= WorldPart->AnimPhases)
					WorldPart->AnimCounter = 0;
			}
		}
		else
			WorldPart->AnimPhase = WorldPart->AnimBase;

	}
}

void CWorldPart_Event_LeaveCurrentSpot(CWorldPart* WorldPart) 
{
	if(WorldPart->Type == IDBox)
	{
		if (WorldParts)
		{
			uint16_t Teller;
			for (Teller=0;Teller< WorldParts->ItemCount;Teller++)
			{
				if( WorldParts->Items[Teller]->Type == IDSpot)
				{
					if ((WorldParts->Items[Teller]->PlayFieldX == WorldPart->PlayFieldX) && (WorldParts->Items[Teller]->PlayFieldY == WorldPart->PlayFieldY))
					{
						WorldPart->AnimPhase = 1;
						break;
					}
				}
			}
		}
	}
}

void CWorldPart_Event_Moving(CWorldPart* WorldPart, int16_t ScreenPosX,int16_t ScreenPosY,int8_t, int8_t) 
{
	if(WorldPart->Type == IDPlayer)
	{
		if((ScreenPosX > (WorldParts->ViewPort->MaxScreenX) - HALFWINDOWWIDTH) && (WorldPart->Xi > 0))
			CViewPort_Move(WorldParts->ViewPort, WorldPart->Xi,WorldPart->Yi);
		if((ScreenPosX < (WorldParts->ViewPort->MaxScreenX) - HALFWINDOWWIDTH) && (WorldPart->Xi < 0))
			CViewPort_Move(WorldParts->ViewPort, WorldPart->Xi,WorldPart->Yi);
		if((ScreenPosY > (WorldParts->ViewPort->MaxScreenY) - HALFWINDOWHEIGHT) && (WorldPart->Yi > 0))
			CViewPort_Move(WorldParts->ViewPort, WorldPart->Xi,WorldPart->Yi);
		if((ScreenPosY < (WorldParts->ViewPort->MaxScreenY) - HALFWINDOWHEIGHT) && (WorldPart->Yi < 0))
			CViewPort_Move(WorldParts->ViewPort, WorldPart->Xi,WorldPart->Yi);
	}
}

void CWorldPart_SetPosition(CWorldPart* WorldPart, const int8_t PlayFieldXin,const int8_t PlayFieldYin)
{
	if ((PlayFieldXin >= 0) && (PlayFieldXin < NrOfCols) && (PlayFieldYin >= 0) && (PlayFieldYin < NrOfRows))
	{
		WorldPart->PlayFieldX=PlayFieldXin;
		WorldPart->PlayFieldY=PlayFieldYin;
		WorldPart->X=PlayFieldXin*TileWidth;
		WorldPart->Y=PlayFieldYin*TileHeight;
		CWorldPart_Event_ArrivedOnNewSpot(WorldPart);
	}
}

void CWorldPart_MoveTo(CWorldPart* WorldPart, const int8_t PlayFieldXin,const int8_t PlayFieldYin,bool BackWards)
{
	if(WorldPart->Type== IDPlayer)
	{
		uint16_t Teller;
		if(!WorldPart->IsMoving)
		{
			if(CWorldPart_CanMoveTo(WorldPart, PlayFieldXin,PlayFieldYin) || BackWards)
			{
				if(WorldParts)
				{
					if(BackWards)
						WorldParts->Moves--;
					else
						WorldParts->Moves++;
				}
				WorldPart->AnimPhases = 4;
				WorldPart->PlayFieldX = PlayFieldXin;
				WorldPart->PlayFieldY = PlayFieldYin;
				if(WorldPart->X < WorldPart->PlayFieldX*TileWidth)
				{
					WorldPart->Xi = WorldPart->MoveSpeed;
					if(BackWards)
					{
						WorldPart->AnimBase = 0;
					}
					else
					{
						WorldPart->AnimBase = 4;
						if (WorldParts)
						{
							for(Teller=0;Teller<WorldParts->ItemCount;Teller++)
							{
								if(((WorldParts->Items[Teller]->Type == IDBox) || (WorldParts->Items[Teller]->Type == IDWall)) && ((WorldParts->Items[Teller]->PlayFieldX == WorldPart->PlayFieldX) && (WorldParts->Items[Teller]->PlayFieldY == WorldPart->PlayFieldY)))
								{
									CWorldPart_MoveTo(WorldParts->Items[Teller], WorldPart->PlayFieldX+1,WorldPart->PlayFieldY,false);
									break;
								}
							}
						}
					}
					
				}
				if(WorldPart->X > WorldPart->PlayFieldX*TileWidth)
				{
					WorldPart->Xi = -WorldPart->MoveSpeed;
					if(BackWards)
					{
						WorldPart->AnimBase = 4;
					}
					else
					{
						WorldPart->AnimBase = 0;
						if (WorldParts)
						{
							for(Teller=0;Teller<WorldParts->ItemCount;Teller++)
							{
								if(((WorldParts->Items[Teller]->Type == IDBox) || (WorldParts->Items[Teller]->Type == IDWall)) && ((WorldPart->PlayFieldX == WorldParts->Items[Teller]->PlayFieldX )  && (WorldParts->Items[Teller]->PlayFieldY == WorldPart->PlayFieldY)))
								{
									CWorldPart_MoveTo(WorldParts->Items[Teller], WorldPart->PlayFieldX-1,WorldPart->PlayFieldY,false);
									break;
								}
							}
						}
						
					}
				}

				if(WorldPart->Y > WorldPart->PlayFieldY*TileHeight)
				{
					WorldPart->Yi = -WorldPart->MoveSpeed;
					if(BackWards)
					{
						WorldPart->AnimBase = 12;
					}
					else
					{
						WorldPart->AnimBase = 8;
						if (WorldParts)
						{
							for(Teller=0;Teller<WorldParts->ItemCount;Teller++)
							{
								if(((WorldParts->Items[Teller]->Type == IDBox) || (WorldParts->Items[Teller]->Type == IDWall)) && ((WorldPart->PlayFieldY == WorldParts->Items[Teller]->PlayFieldY)  && (WorldParts->Items[Teller]->PlayFieldX == WorldPart->PlayFieldX)))
								{
									CWorldPart_MoveTo(WorldParts->Items[Teller], WorldPart->PlayFieldX,WorldPart->PlayFieldY-1,false);
									break;
								}						
							}
						}
					}
				}
				if(WorldPart->Y < WorldPart->PlayFieldY*TileHeight)
				{
					WorldPart->Yi = WorldPart->MoveSpeed;
					if(BackWards)
					{
						WorldPart->AnimBase = 8;
					}
					else
					{
						WorldPart->AnimBase = 12;
						if(WorldParts)
						{
							for(Teller=0;Teller<WorldParts->ItemCount;Teller++)
							{
								if(((WorldParts->Items[Teller]->Type == IDBox) || (WorldParts->Items[Teller]->Type == IDWall)) && ((WorldParts->Items[Teller]->PlayFieldY == WorldPart->PlayFieldY)  && (WorldParts->Items[Teller]->PlayFieldX == WorldPart->PlayFieldX )))
								{
									CWorldPart_MoveTo(WorldParts->Items[Teller], WorldPart->PlayFieldX,WorldPart->PlayFieldY+1,false);
									break;
								}
							}
						}
					}
				}
				playGameMoveSound();
				WorldPart->IsMoving = true;
			}
			else
			{
				WorldPart->AnimPhases = 0;
				WorldPart->AnimCounter = 0;

				if (PlayFieldXin > WorldPart->PlayFieldX)
				{
					WorldPart->AnimBase= 4;
				}
				if (PlayFieldXin < WorldPart->PlayFieldX)
				{
					WorldPart->AnimBase = 0;
				}
				if (PlayFieldYin > WorldPart->PlayFieldY)
				{
					WorldPart->AnimBase = 12;
				}
				if (PlayFieldYin < WorldPart->PlayFieldY)
				{
					WorldPart->AnimBase = 8;
				}
				WorldPart->AnimPhase = WorldPart->AnimBase + WorldPart->AnimCounter;
				WorldPart->AnimDelayCounter++;
				if (WorldPart->AnimDelayCounter >= WorldPart->AnimDelay)
				{
					WorldPart->AnimDelayCounter = 0;
					WorldPart->AnimCounter++;
					if (WorldPart->AnimCounter >= WorldPart->AnimPhases)
						WorldPart->AnimCounter = 0;
				}
			}
		}
	}
	else
	{
		if(!WorldPart->IsMoving)
		{
			if((PlayFieldXin != WorldPart->PlayFieldX) || (PlayFieldYin != WorldPart->PlayFieldY))
				if(CWorldPart_CanMoveTo(WorldPart,PlayFieldXin,PlayFieldYin) || BackWards)
				{
					if(WorldPart->Type == IDBox)
					{
						if(WorldParts)
						{
							if(BackWards)
								WorldParts->Pushes--;
							else
								WorldParts->Pushes++;
						}
					}
					WorldPart->PlayFieldX = PlayFieldXin;
					WorldPart->PlayFieldY = PlayFieldYin;
					if(WorldPart->X < WorldPart->PlayFieldX*TileWidth)
						WorldPart->Xi = WorldPart->MoveSpeed;
					if(WorldPart->X > WorldPart->PlayFieldX*TileWidth)
						WorldPart->Xi = -WorldPart->MoveSpeed;
					if(WorldPart->Y > WorldPart->PlayFieldY*TileHeight)
						WorldPart->Yi = -WorldPart->MoveSpeed;
					if(WorldPart->Y < WorldPart->PlayFieldY*TileHeight)
						WorldPart->Yi = WorldPart->MoveSpeed;
					WorldPart->IsMoving = true;
					CWorldPart_Event_LeaveCurrentSpot(WorldPart);
				}
		}
	}
}

bool CWorldPart_CanMoveTo(CWorldPart* WorldPart, const int8_t PlayFieldXin,const int8_t PlayFieldYin) 
{
	bool Result = true;
	uint16_t Teller;
	if(WorldPart->Type == IDBox)
	{
		if ((PlayFieldXin >= 0) && (PlayFieldXin < NrOfCols) && (PlayFieldYin >= 0) && (PlayFieldYin < NrOfRows))
		{
			if (WorldParts)
			{
				for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
					if((WorldParts->Items[Teller]->Type == IDWall) || (WorldParts->Items[Teller]->Type == IDBox))
						if((WorldParts->Items[Teller]->PlayFieldX == PlayFieldXin) && (WorldParts->Items[Teller]->PlayFieldY == PlayFieldYin))
						{
							Result = false;
							break;
						}
			}
		}
		else
			Result = false;
		return Result;

	}
	else
	{
		if(WorldPart->Type == IDPlayer)
		{
			if ((PlayFieldXin >= 0) && (PlayFieldXin < NrOfCols) && (PlayFieldYin >= 0) && (PlayFieldYin < NrOfRows))
			{
				if (WorldParts)
				{
					for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
						if((WorldParts->Items[Teller]->PlayFieldX == PlayFieldXin) && (WorldParts->Items[Teller]->PlayFieldY == PlayFieldYin))
						{
							if(WorldParts->Items[Teller]->Type == IDWall)
							{
								Result = false;
								break;
							}
							if(WorldParts->Items[Teller]->Type == IDBox)
							{
								if (WorldPart->PlayFieldX > PlayFieldXin)
								{
									Result = CWorldPart_CanMoveTo(WorldParts->Items[Teller], PlayFieldXin-1,PlayFieldYin);
								}
								if (WorldPart->PlayFieldX < PlayFieldXin)
								{
									Result = CWorldPart_CanMoveTo(WorldParts->Items[Teller], PlayFieldXin+1,PlayFieldYin);
								}
								if (WorldPart->PlayFieldY > PlayFieldYin)
								{
									Result = CWorldPart_CanMoveTo(WorldParts->Items[Teller], PlayFieldXin,PlayFieldYin-1);
								}
								if (WorldPart->PlayFieldY < PlayFieldYin)
								{
									Result = CWorldPart_CanMoveTo(WorldParts->Items[Teller],PlayFieldXin,PlayFieldYin+1);
								}
								break;
							}

						}
				}
			}
			else
				Result = false;
			return Result;

		}
		else
			return false;
	}
}

void CWorldPart_Move(CWorldPart* WorldPart)
{
	if (!WorldPart->FirstArriveEventFired)
	{
		CWorldPart_Event_ArrivedOnNewSpot(WorldPart);
		WorldPart->FirstArriveEventFired=true;
	}
	if (WorldPart->IsMoving)
	{
		if (WorldPart->MoveDelayCounter == WorldPart->MoveDelay)
		{
			WorldPart->X += WorldPart->Xi;
			WorldPart->Y += WorldPart->Yi;
			CWorldPart_Event_Moving(WorldPart, WorldPart->X,WorldPart->Y,WorldPart->Xi,WorldPart->Yi);
			if ((WorldPart->X == WorldPart->PlayFieldX * TileWidth) && (WorldPart->Y == WorldPart->PlayFieldY * TileHeight))
			{
				WorldPart->IsMoving = false;
				WorldPart->Xi = 0;
				WorldPart->Yi = 0;
				CWorldPart_Event_ArrivedOnNewSpot(WorldPart);
			}
			WorldPart->MoveDelayCounter = -1;
		}
		WorldPart->MoveDelayCounter++;
	}
}

//the image for the frame this part is currently showing. Every type is drawn the
//same way, an 8x8 magenta keyed sprite taken from its sheet at AnimPhase
const uint8_t* CWorldPart_SpriteData(CWorldPart* WorldPart)
{
	const uint8_t* base;
	switch (WorldPart->Type)
	{
		case IDPlayer:         base = IMGPlayer;    break;
		case IDBox:            base = IMGBox;       break;
		case IDSpot:           base = IMGSpot;      break;
		case IDFloor:          base = IMGFloor;     break;
		case IDWall:           base = IMGWall;      break;
		default: return NULL;
	}
	return base + WorldPart->AnimPhase * TileWidth * TileHeight * sizeof(uint16_t);
}

void CWorldPart_Draw(CWorldPart* WorldPart)
{
	CWorldPart_Event_BeforeDraw(WorldPart);
	DrawImageTransparent(WorldPart->X - WorldParts->ViewPort->MinScreenX, WorldPart->Y - WorldParts->ViewPort->MinScreenY,
	                     TileWidth, TileHeight, CWorldPart_SpriteData(WorldPart));
}


void CWorldPart_Destroy(CWorldPart* WorldPart)
{
	if(WorldPart)
	{
		if(WorldPart->BHistory)
			CHistory_Destroy(WorldPart->History);
		WorldPart->Type = 0;
	}
}
