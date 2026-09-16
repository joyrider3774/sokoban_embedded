#include <string.h>
#include <stdio.h>
#include "Credits.h"
#include "Common.h"
#include "GameFuncs.h"
#include "Sound.h"

//the text never changes, so it is drawn straight from the literal instead of a copy in RAM
static const char* const CreditsTekst = "Creator:\nWillems Davy\nWillems Soft 2026\njoyrider3774.itch.io";

void CreditsInit()
{
	//this screen is about to be rebuilt, drop any cached draw signature
	ScreenForceRedraw();
}

void Credits()
{
	
	if (GameState == GSCreditsInit)
	{
		CreditsInit();
		GameState = GSCredits;
	}

	if (((currButtons & BUTTON_A) && !(prevButtons & BUTTON_A)) ||
		((currButtons & BUTTON_B) && !(prevButtons & BUTTON_B)))
	{
		playMenuBackSound();
		GameState = GSTitleScreenInit;
	}
	if (ScreenChanged(GameState * 100000))
	{
		pushImageRLE(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, IMGTitleScreen);
		GFX.fillRect(0, 30, 128, 69, ColorWhite);
		GFX.drawRect(0, 30, 128, 69, ColorBlack);
		GFX.drawRect(2, 32, 124, 65, ColorBlack);

		tftPrint(4, 34, CreditsTekst, ColorBlack, ColorBlack, 1);

		printTitleInfo();
	}

	
}
