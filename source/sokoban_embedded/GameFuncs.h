#ifndef GAMEFUNCS_H
#define GAMEFUNCS_H

#include <stdint.h>
#include <stddef.h>

void LoadGraphics(void);
uint8_t CurrentSkin(void);
void DrawImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t* image);
//the same, magenta pixels are left out
void DrawImageTransparent(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t* image);
//draws a run length encoded full screen image (tools/png2rle565.py)
void pushImageRLE(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t* data);
//only exists with a screen buffer: draws an image into it, transparent skips magenta pixels
void DrawImageToBuffer(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t* image, bool transparent);
void FindLevels();
//signatures are built as GameState * 100000 + ..., too big for 16 bits
bool ScreenChanged(int32_t signature);
void ScreenForceRedraw();
//a char is signed on some compilers and unsigned on others, int16_t holds both ranges
int16_t ord(char chr);
char chr(uint8_t ascii);
uint16_t MaxLineLen(const char* Text);
void tftPrint(int16_t x, int16_t y, const char* str, uint16_t color, uint16_t bg, uint8_t size);
void printTitleInfo();
void LoadUnlockData();
void SaveUnlockData();
void AskQuestion(int8_t Id, const char* Msg);
bool AskQuestionUpdate(int8_t* Id, bool* Answer, bool MustBeAButton);
void SearchForLevelPacks();
void SaveSettings();
void LoadSettings();

#endif