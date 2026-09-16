#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Common.h"
#include "GameFuncs.h"
#include "Sound.h"
#include "CWorldPart.h"

bool FreeView = false;

bool StageDone()
{
	uint16_t Teller,FilledSpots=0,Spots=0;
	for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
		if (WorldParts->Items[Teller]->Type == IDBox)
		{
			if (WorldParts->Items[Teller]->AnimPhase == 1)
                FilledSpots++;
		}
		else
            if (WorldParts->Items[Teller]->Type == IDSpot)
                Spots++;
    if (FilledSpots >= Spots)
        return true;
    else
        return false;
}

void RestartLevel()
{
	if (!AskingQuestion)
	{
		playMenuSelectSound();
		CWorldParts_DrawBoard(WorldParts);
		AskQuestion(IDRestartLevel, "You are about to\nrestart this level\nAre you sure you\nwant to restart?\n\n(A)Restart (B)Cancel");
	}
}

void ShowGameInfo()
{
	if (!AskingQuestion)
	{
		char Msg[200];
		playMenuSelectSound();
		CWorldParts_Draw(WorldParts);
		if (LevelPackFile->Loaded)
		{
			snprintf(Msg, 200, "Level Pack: %s\nLevel: %d/%d - Moves: %" PRIu32 " - Pushes: %" PRId32 "\nAuthor: %s\nComments: %s", LevelPackName, SelectedLevel, InstalledLevels, WorldParts->Moves, WorldParts->Pushes, LevelPackFile->Meta.author, LevelPackFile->Meta.comments);
		}
		AskQuestion(IDLevelInfo, Msg);
	}
}

void GameInit()
{
	//this screen is about to be rebuilt, drop any cached draw signature
	ScreenForceRedraw();
	uint16_t Teller;
	//whatever the previous state left on screen has to go
	CWorldParts_MarkAllDirty();
	CWorldPart *Player=NULL;

	for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
	{
		if (WorldParts->Items[Teller]->Type == IDPlayer)
		{
			Player = WorldParts->Items[Teller];
			break;
		}
	}
	//should never happen
	if(!Player)
	{
		Player = CWorldPart_Create(0,0, true, IDPlayer);
		CWorldParts_Add(WorldParts,Player);
		CWorldParts_LimitVPLevel(WorldParts);
	}

}

void Game()
{
	char Msg[200];
	if(GameState == GSGameInit)
	{
		GameInit();
		GameState = GSGame;
	}
	bool response = false;
	int8_t id = -1;
	if(!AskingQuestion)
	{
		if (!WorldParts->Player->IsMoving && (WorldParts->Moves >0) && StageDone())
		{
			playLevelDoneSound();
			if (SelectedLevel == UnlockedLevels)
			{
				if ( UnlockedLevels < InstalledLevels)
				{
					snprintf(Msg, sizeof(Msg),"Congratulations !\nYou Solved\nLevel %d/%d\nThe next level has\nnow been unlocked!\n(A) Continue",SelectedLevel,InstalledLevels);
					AskQuestion(IDSolvedLevelNextUnlocked, Msg);
				}
				else
				{
					snprintf(Msg, sizeof(Msg),"Congratulations !\nYou Solved\nLevel %d/%d\nlevelpack %s\nis now finished,\ntry out another one!\n(A) Continue",SelectedLevel,InstalledLevels,LevelPackName);
					AskQuestion(IDSolvedLastLevel, Msg);
						
				}
			}
			else
			{
				snprintf(Msg, sizeof(Msg),"Congratulations !\nYou Solved\nLevel %d/%d\n\n(A) Continue",SelectedLevel,InstalledLevels);
				AskQuestion(IDSolvedEarlierLevel, Msg);
			}
		}
		
		if (!FreeView && ((currButtons & BUTTON_B) && (!(prevButtons & BUTTON_B))))
		{
			playMenuBackSound();
			AskQuestion(IDQuitPlaying, "Quit playing the\ncurrent level and\nreturn to the level\nselector?\n\n(A) Quit (B) Cancel");
		}

		//restart
		if ((currButtons & BUTTON_L) && (!(prevButtons & BUTTON_L)))
		{
			RestartLevel();
		}

		//freeview
		if (!FreeView && (currButtons & BUTTON_R) && (!(prevButtons & BUTTON_R)))
		{
			playMenuSelectSound();
			FreeView = true;
			prevButtons = currButtons;
		}

		if (FreeView)
		{
			if (((currButtons & BUTTON_B) && (!(prevButtons & BUTTON_B))) ||
				((currButtons & BUTTON_R) && (!(prevButtons & BUTTON_R))))
			{
				playMenuBackSound();
				FreeView = false;
				CWorldParts_CenterVPOnPlayer(WorldParts);
			}

			//the board notices the viewport scrolled and repaints everything by itself
			if (FreeView)
			{
				if (currButtons & BUTTON_LEFT)
					CViewPort_Move(WorldParts->ViewPort, -ViewportMove, 0);
				if (currButtons & BUTTON_RIGHT)
					CViewPort_Move(WorldParts->ViewPort, ViewportMove, 0);
				if (currButtons & BUTTON_UP)
					CViewPort_Move(WorldParts->ViewPort, 0, -ViewportMove);
				if (currButtons & BUTTON_DOWN)
					CViewPort_Move(WorldParts->ViewPort, 0, ViewportMove);
			}

		}
		else
		{
			if (!WorldParts->Player->IsMoving)
			{
				if (currButtons & BUTTON_A)
				{
					CWorldParts_HistoryGoBack(WorldParts);
				}
				
				if (!(currButtons & BUTTON_A) && (currButtons & BUTTON_RIGHT))
				{
					if (CWorldPart_CanMoveTo(WorldParts->Player, WorldParts->Player->PlayFieldX + 1, WorldParts->Player->PlayFieldY))
					{
						CWorldParts_HistoryAdd(WorldParts);
					}
					CWorldPart_MoveTo(WorldParts->Player, WorldParts->Player->PlayFieldX + 1, WorldParts->Player->PlayFieldY, false);
				}
				else
				{
					if (!(currButtons & BUTTON_A) && (currButtons & BUTTON_LEFT))
					{
						if (CWorldPart_CanMoveTo(WorldParts->Player, WorldParts->Player->PlayFieldX - 1, WorldParts->Player->PlayFieldY))
						{
							CWorldParts_HistoryAdd(WorldParts);
						}
						CWorldPart_MoveTo(WorldParts->Player, WorldParts->Player->PlayFieldX - 1, WorldParts->Player->PlayFieldY, false);
					}
					else
					{
						if (!(currButtons & BUTTON_A) && (currButtons & BUTTON_UP))
						{
							if (CWorldPart_CanMoveTo(WorldParts->Player, WorldParts->Player->PlayFieldX, WorldParts->Player->PlayFieldY - 1))
							{
								CWorldParts_HistoryAdd(WorldParts);
							}
							CWorldPart_MoveTo(WorldParts->Player, WorldParts->Player->PlayFieldX, WorldParts->Player->PlayFieldY - 1, false);
						}
						else
						{
							if (!(currButtons & BUTTON_A) && (currButtons & BUTTON_DOWN))
							{
								if (CWorldPart_CanMoveTo(WorldParts->Player, WorldParts->Player->PlayFieldX, WorldParts->Player->PlayFieldY + 1))
								{
									CWorldParts_HistoryAdd(WorldParts);
								}
								CWorldPart_MoveTo(WorldParts->Player, WorldParts->Player->PlayFieldX, WorldParts->Player->PlayFieldY + 1, false);
							}
						}
					}
				}
			}			
		}

		if(!AskingQuestion)
		{
			CWorldParts_Move(WorldParts);
			CWorldParts_DrawBoard(WorldParts);
			//the top bar sits over the board, so it is repainted whenever the board was
			if (FreeView)
			{
				GFX.fillRect(0, 0, WINDOW_WIDTH, 12, ColorWhite);
				GFX.drawRect(0, 11, WINDOW_WIDTH, 1, ColorBlack);
				tftPrint(2,2, "dpad:Move B:exit", ColorBlack, ColorBlack,1);
			}
		}
	}
	else
	{	
		if(AskQuestionUpdate(&id, &response, false))
		{
			if(id == IDSolvedLevelNextUnlocked)
			{
				UnlockedLevels++;
				SelectedLevel++;
				SaveUnlockData();
				GameState = GSStageSelectInit;
			}

			if(id == IDSolvedLastLevel)
			{
				GameState = GSTitleScreenInit;
			}

			if (id == IDSolvedEarlierLevel)
			{
				GameState = GSStageSelectInit;
			}

			if (id == IDRestartLevel)
			{
				if(response)
				{
					CWorldParts_LoadFromLevelPackFile(WorldParts, LevelPackFile, SelectedLevel, true);
					CWorldPart *Player=NULL;
					for (uint16_t Teller=0;Teller<WorldParts->ItemCount;Teller++)
					{
						if (WorldParts->Items[Teller]->Type == IDPlayer)
						{
							Player = WorldParts->Items[Teller];
							break;
						}
					}
					//should never happen
					if(!Player)
					{
						Player = CWorldPart_Create(0,0, true, IDPlayer);
						CWorldParts_Add(WorldParts,Player);
						CWorldParts_LimitVPLevel(WorldParts);
					}
					CWorldParts_MarkAllDirty();
					FreeView = false;
				}
			}

			if (id == IDQuitPlaying)
			{
				//keep playing needs no repaint here, closing the question marked the board dirty
				if (response)
				{
					GameState = GSStageSelectInit;
				}
			}
		}
	}
}
