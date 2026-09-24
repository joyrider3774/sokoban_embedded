#ifndef DEFINES_H
#define DEFINES_H

//the device comes first: the display library, SCREENBUFFER and IMAGESET are device settings, see
//PlatformESPboy.h / PlatformSDL.h
#include "PlatformDevice.h"

#define WINDOW_WIDTH 128
#define WINDOW_HEIGHT 128
#define HALFWINDOWWIDTH 64
#define HALFWINDOWHEIGHT 64
#define FPS 30
//1 = every frame waits until 1/FPS of a second has passed, 0 = a frame starts as soon
//as the last one is done, to see how fast the game can go. Movement, animation, input
//repeat and music all count frames, so without the lock they run faster as well.
//A build can set it itself
#ifndef FPSLOCK
#define FPSLOCK 1
#endif
//1 = the debug header (frame rate, free heap and stack) is always shown, Up + Down does not
//hide it. 0 = it starts hidden and Up + Down shows and hides it. A build can set it itself
#ifndef FORCEDEBUG
#define FORCEDEBUG 0
#endif
//1 = the colours of an image are spread over the ones the buffer can hold, so that a shade it
//has no colour for is a pattern of the two it does instead of the nearer of them. 0 = every
//colour becomes the nearest one there is, which shows as bands across anything that shades.
//An 8 bpp buffer is RGB332 and drops 2 bits of red, 3 of green and 3 of blue, and a 1 bpp buffer
//keeps only black and white, so both have something to spread. A 16 bpp buffer holds every colour
//of the image as it is and is left alone. A build can set this itself, see DitherSpread in
//Platform.h
#ifndef DITHERING
#define DITHERING 0
#endif

//1 = the floor tiles the player can reach are found with a floodfill every frame and
//drawn under the level, 0 = no floor is drawn and the floodfill, its bitmaps and its tile
//stack are left out of the build. A build can set it itself
#ifndef FLOODFILLFLOOR
#define FLOODFILLFLOOR 1
#endif
#define MAXSKINS 2
//the only skin a 1 bpp buffer can show
#define SKINBLACKWHITE 1

//FORCESKIN: -1 = every skin is built in and can be picked in the options, n = only skin n
//(0 default, 1 black & white) is built in and always used, which saves the flash of the others on a
//small device. A 1 bpp buffer has only two colours to show, so the
//black & white skin is the one it takes on its own. A build can still ask it for another one,
//whose shades then go through the brightness rule in SetBufferBit, and with DITHERING come out
//as a pattern of the two colours rather than as the nearer of them.
//Set by the device header or the build
#if !defined(FORCESKIN)
  #if SCREENBUFFER == 1
  #define FORCESKIN SKINBLACKWHITE
  #else
  #define FORCESKIN -1
  #endif
#endif
//1 when the images of skin n are part of the build
#define SKINBUILT(n) ((FORCESKIN < 0) || (FORCESKIN == (n)))

//image headers to build with: 1 = images (16x16 tiles), 2 = images2 (same images at half size, 8x8 tiles)
//set by the device header (PlatformESPboy.h / PlatformSDL.h) or by the build
#ifndef IMAGESET
#error "the device header has to define IMAGESET"
#endif

#if IMAGESET == 2
#define IMAGES_DIR images2
#define TileWidth 8
#define TileHeight 8
#define GameMoveSpeed 2					//dec if fps increases
#define PlayerAnimDelay 3				//inc if fps increases
#define ViewportMove 2					//dec if fps increases
#else
#define IMAGES_DIR images
#define TileWidth 16
#define TileHeight 16
#define GameMoveSpeed 4					//dec if fps increases
#define PlayerAnimDelay 3				//inc if fps increases
#define ViewportMove 4					//dec if fps increases
#endif

//path of an image header in the selected folder: #include GAME_IMAGE(box_RGB565_LE.h)
#define GAME_IMAGE_STR(x) #x
#define GAME_IMAGE_PATH(dir, file) GAME_IMAGE_STR(dir/file)
#define GAME_IMAGE(file) GAME_IMAGE_PATH(IMAGES_DIR, file)
#define NrOfRows 16
#define NrOfCols 25
#define NrOfColsVisible (WINDOW_WIDTH / TileWidth)
#define NrOfRowsVisible ((WINDOW_HEIGHT / TileHeight))
#define MaxHistory 25
#define ZEmpty 6
#define ZPlayer 5
#define ZBox 4
#define ZWall 3
#define ZSpot 2
#define ZFloor 1
#define IDPlayer 1
#define IDBox 2
#define IDWall 3
#define IDSpot 4
#define IDEmpty 5
#define IDFloor 6

#define MAXWORLDPARTS ((NrOfCols * NrOfRows)+2)
//only the parts that actually moved this frame get redrawn on top, that is the
//active player plus whatever it is pushing
#define MAXMOVEABLEWORLDPARTS 8
#define MaxLevelPacks 19

//LEVELPACKS: the level packs that are built in, an LP_ bit each (the size is what the pack takes
//in flash). All of them unless the device header or the build picks fewer, a pack that is left
//out takes no flash and does not show up in the game
#define LP_696                     (1ul <<  0)    //696.sok                      69250 bytes
#define LP_Cosmonotes              (1ul <<  1)    //Cosmonotes.sok                4603 bytes
#define LP_Cosmopoly               (1ul <<  2)    //Cosmopoly.sok                 5117 bytes
#define LP_Erim_Sever_Collection   (1ul <<  3)    //Erim Sever Collection.sok    33620 bytes
#define LP_GRIGoRusha_2001         (1ul <<  4)    //GRIGoRusha 2001.sok          17356 bytes
#define LP_GRIGoRusha_2002         (1ul <<  5)    //GRIGoRusha 2002.sok           7931 bytes
#define LP_GRIGoRusha_Remodel_Club (1ul <<  6)    //GRIGoRusha Remodel Club.sok  37304 bytes
#define LP_GRIGoRusha_Special      (1ul <<  7)    //GRIGoRusha Special.sok        7963 bytes
#define LP_GRIGoRusha_Star         (1ul <<  8)    //GRIGoRusha Star.sok           6130 bytes
#define LP_GRIGoRusha_Sun          (1ul <<  9)    //GRIGoRusha Sun.sok            2351 bytes
#define LP_LOMA                    (1ul << 10)    //LOMA.sok                     13983 bytes
#define LP_Microcosmos             (1ul << 11)    //Microcosmos.sok               8057 bytes
#define LP_Minicosmos              (1ul << 12)    //Minicosmos.sok                7844 bytes
#define LP_Myriocosmos             (1ul << 13)    //Myriocosmos.sok               3823 bytes
#define LP_Nabokosmos              (1ul << 14)    //Nabokosmos.sok                8084 bytes
#define LP_Picokosmos              (1ul << 15)    //Picokosmos.sok                4572 bytes
#define LP_SokEvo                  (1ul << 16)    //SokEvo.sok                   13399 bytes
#define LP_SokHard                 (1ul << 17)    //SokHard.sok                  30319 bytes
#define LP_SokWhole                (1ul << 18)    //SokWhole.sok                 13246 bytes
#define LP_ALL ((1ul << 19) - 1)
#ifndef LEVELPACKS
#define LEVELPACKS LP_ALL
#endif
#if (LEVELPACKS & LP_ALL) == 0
#error "LEVELPACKS has to leave at least one level pack in"
#endif
#define InputDelay 16
#define MaxLevelPackNameLength 50

#define GSTitleScreen 1 
#define GSCredits 2
#define GSGame 3 
#define GSStageSelect 4
#define GSOptions 5

#define GSTitleScreenInit 51
#define GSCreditsInit 52
#define GSGameInit 53
#define GSStageSelectInit 54
#define GSOptionsInit 55

#define IDSolvedLevelNextUnlocked 2
#define IDSolvedLastLevel 3
#define IDSolvedEarlierLevel 4
#define IDNoPlayer 5
#define IDMoreSpotsThanBoxes 6
#define IDNoLevelsInPack 7
#define IDCurrentLevelNotSaved 8
#define IDRestartLevel 9
#define IDDeleteAllParts 10
#define IDDeleteLevelPack 11
#define IDDeleteLevel 12
#define IDLevelNotUnlocked 13
#define IDLevelInfo 14
#define IDQuitPlaying 15


#define MenuUpdateTicks 10
//frames between repeats while a direction is held in the level selector
#define LevelSelectUpdateTicks 5

#endif